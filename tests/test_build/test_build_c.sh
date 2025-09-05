#!/usr/bin/env bash
set -euo pipefail

# constants
readonly script_dir="$(dirname "$(readlink -f "$0")")"
readonly project_root="$(cd "$script_dir/../.." && pwd)"
readonly log_dir="$script_dir/logs"

# version requirements
readonly min_gcc_version=7
readonly min_autoconf_major=2
readonly min_autoconf_minor=64

total_tests=0
passed_tests=0
skipped_tests=0
failed_tests=0
hdf5_test_path="${HDF5_TEST_PATH:-}"
test_filter="all"

cleanup() {
    cd "$project_root" 2>/dev/null || true
    if [[ -f Makefile ]]; then
        make distclean >/dev/null 2>&1 || true
    fi
}

# register cleanup on exit
trap cleanup EXIT

check_prerequisites() {
    local errors=0

    # check gcc
    if ! command -v gcc >/dev/null 2>&1; then
        echo "ERROR: gcc not found"
        errors=$((errors + 1))
    else
        local gcc_version
        gcc_version=$(gcc -dumpversion | cut -d. -f1)
        if [[ "$gcc_version" -lt $min_gcc_version ]]; then
            echo "ERROR: gcc version $gcc_version < $min_gcc_version"
            errors=$((errors + 1))
        fi
    fi

    # check autoconf
    if ! command -v autoconf >/dev/null 2>&1; then
        echo "ERROR: autoconf not found"
        errors=$((errors + 1))
    else
        local autoconf_version
        autoconf_version=$(autoconf --version | head -n1 | grep -oE '[0-9]+\.[0-9]+' | head -n1)
        local major minor
        IFS='.' read -r major minor <<< "$autoconf_version"
        if [[ "$major" -eq $min_autoconf_major && "$minor" -lt $min_autoconf_minor ]]; then
            echo "ERROR: autoconf version $autoconf_version < $min_autoconf_major.$min_autoconf_minor"
            errors=$((errors + 1))
        fi
    fi

    # check automake
    if ! command -v automake >/dev/null 2>&1; then
        echo "ERROR: automake not found"
        errors=$((errors + 1))
    fi

    # no error should return exit code 0 to indicate success
    return "$errors"
}

# check if test should be run based on filter
should_run_test() {
    local test_name="$1"
    local filter="$2"
    [[ "$filter" == "all" ]] || [[ "$filter" == "$test_name" ]]
}

# run test in an isolated environment
run_isolated_test() {
    local test_name="$1"
    local configure_cmd="$2"
    local env_setup="${3:-}"

    total_tests=$((total_tests + 1))
    echo -n "Test: $test_name... "

    # run in subshell for complete isolation
    if (
        # clear all HDF5 env vars first
        unset HDF5_ROOT HDF5_HOME HDF5_DIR
        # apply test-specific environment if provided
        if [[ -n "$env_setup" ]]; then
            # parse space-separated var=value pairs
            local -a env_pairs
            IFS=' ' read -ra env_pairs <<< "$env_setup"
            for pair in "${env_pairs[@]}"; do
                # strip 'export ' if present
                pair="${pair#export }"
                # only export valid var=value pairs
                if [[ "$pair" == *=* ]]; then
                    export "$pair"
                fi
            done
        fi
        # run the actual test
        run_test_internal "$test_name" "$configure_cmd"
    ); then
        passed_tests=$((passed_tests + 1))
    else
        failed_tests=$((failed_tests + 1))
    fi
}

# actual test runner
run_test_internal() {
    local test_name="$1"
    local configure_cmd="$2"
    local log_file="$log_dir/${test_name}.log"

    # clean previous build
    cleanup

    # parse the command into an array
    local -a cmd_array
    IFS=' ' read -ra cmd_array <<< "$configure_cmd"

    # run configure (from an array of arguments)
    if ! (cd "$project_root" && "${cmd_array[@]}" >>"$log_file" 2>&1); then
        # attempt to extract the actual error message
        local error_line=$(grep -E "error:|not found at" "$log_file" | tail -1 | sed 's/^configure: //')
        if [[ -n "$error_line" ]]; then
            echo "FAIL ($error_line)"
        else
            echo "FAIL (configure)"
        fi
        return 1
    fi

    # make
    if ! (cd "$project_root" && make >>"$log_file" 2>&1); then
        # attempt to extract make error
        local error_line=$(grep -E "^make.*Error|error:" "$log_file" | tail -1 | head -c 60)
        if [[ -n "$error_line" ]]; then
            echo "FAIL (make: $error_line...)"
        else
            echo "FAIL (make)"
        fi
        return 1
    fi

    # test executable
    local mib2h5="$project_root/src/mib2h5"
    if [[ ! -f "$mib2h5" ]]; then
        echo "FAIL (executable mib2h5 not found)"
        return 1
    fi

    # determine hdf5 library path for LD_LIBRARY_PATH
    local hdf5_lib=""
    if [[ -n "${HDF5_ROOT:-}" ]]; then
        hdf5_lib="$HDF5_ROOT"
    elif [[ -n "${HDF5_HOME:-}" ]]; then
        hdf5_lib="$HDF5_HOME"
    elif [[ -n "${HDF5_DIR:-}" ]]; then
        hdf5_lib="$HDF5_DIR"
    elif [[ -n "$hdf5_test_path" ]] && [[ "$test_name" == "explicit_path" ]]; then
        hdf5_lib="$hdf5_test_path"
    fi

    local ld_path=""
    if [[ -n "$hdf5_lib" ]]; then
        ld_path="LD_LIBRARY_PATH=$hdf5_lib/lib:${LD_LIBRARY_PATH:-}"
    fi

    # test basic commands
    if ! (cd "$project_root" && env "$ld_path" "$mib2h5" --version >>"$log_file" 2>&1); then
        echo "FAIL (--version)"
        return 1
    fi

    if ! (cd "$project_root" && env "$ld_path" "$mib2h5" --help >>"$log_file" 2>&1); then
        echo "FAIL (--help)"
        return 1
    fi

    echo "PASS"
    return 0
}


main() {
    # parse arguments
    case "${1:-}" in
        --clean)
            echo "Cleaning build artefacts..."
            cleanup
            rm -rf "$log_dir"
            echo "Done"
            exit 0
            ;;
        --list)
            echo "Available tests:"
            echo "  system    - Test with system HDF5 installation"
            echo "  explicit  - Test with explicit --with-hdf5 path (requires HDF5_TEST_PATH)"
            echo "  hdf5_root - Test with HDF5_ROOT environment variable"
            echo "  hdf5_home - Test with HDF5_HOME environment variable"
            echo "  hdf5_dir  - Test with HDF5_DIR environment variable"
            echo ""
            echo "Usage:"
            echo "  $0              # Run all tests"
            echo "  $0 --only TEST  # Run specific test"
            echo "  $0 --list       # Show this list"
            echo "  $0 --clean      # Clean build artefacts"
            exit 0
            ;;
        --only)
            if [[ -z "${2:-}" ]]; then
                echo "Error: --only requires a test name"
                echo "Use --list to see available tests"
                exit 1
            fi
            test_filter="$2"
            ;;
        *)
            if [[ -n "${1:-}" ]]; then
                echo "Unknown option: $1"
                echo "Use --list to see available options"
                exit 1
            fi
            ;;
    esac

    echo "=== C Build Tests ==="
    if [[ "$test_filter" != "all" ]]; then
        echo "Running only: $test_filter"
    fi
    echo

    # check prerequisites
    echo -n "Prerequisites check... "
    if check_prerequisites >/dev/null 2>&1; then
        echo "OK"
    else
        echo "FAIL"
        # re-run to show stdout
        check_prerequisites
        exit 1
    fi
    echo

    # create log directory
    mkdir -p "$log_dir"

    # run autoreconf once
    echo -n "Running autoreconf -i... "
    if (cd "$project_root" && autoreconf -i >/dev/null 2>&1); then
        echo "OK"
    else
        echo "FAIL"
        exit 1
    fi
    echo

    # test 1: system hdf5
    if should_run_test "system" "$test_filter"; then
        run_isolated_test "system_hdf5" "./configure" ""
    fi

    # test 2: explicit path
    if should_run_test "explicit" "$test_filter"; then
        if [[ -n "$hdf5_test_path" ]]; then
            run_isolated_test "explicit_path" "./configure --with-hdf5=$hdf5_test_path" ""
        else
            skipped_tests=$((skipped_tests + 1))
            echo "Test: explicit_path... SKIP (set HDF5_TEST_PATH to test)"
        fi
    fi

    # test 3: hdf5_root environment variable
    if should_run_test "hdf5_root" "$test_filter"; then
        if [[ -n "${HDF5_ROOT:-}" ]]; then
            run_isolated_test "hdf5_root" "./configure" "HDF5_ROOT=$HDF5_ROOT" || true
        else
            skipped_tests=$((skipped_tests + 1))
            echo "Test: hdf5_root... SKIP (HDF5_ROOT not set)"
        fi
    fi

    # test 4: hdf5_home environment variable
    if should_run_test "hdf5_home" "$test_filter"; then
        if [[ -n "${HDF5_HOME:-}" ]]; then
            run_isolated_test "hdf5_home" "./configure" "HDF5_HOME=$HDF5_HOME" || true
        else
            skipped_tests=$((skipped_tests + 1))
            echo "Test: hdf5_home... SKIP (HDF5_HOME not set)"
        fi
    fi

    # test 5: hdf5_dir environment variable
    if should_run_test "hdf5_dir" "$test_filter"; then
        if [[ -n "${HDF5_DIR:-}" ]]; then
            run_isolated_test "hdf5_dir" "./configure" "HDF5_DIR=$HDF5_DIR" || true
        else
            skipped_tests=$((skipped_tests + 1))
            echo "Test: hdf5_dir... SKIP (HDF5_DIR not set)"
        fi
    fi

    # summary
    echo

    # handle case where no tests ran
    if [[ $total_tests -eq 0 ]] && [[ $skipped_tests -gt 0 ]]; then
        echo "Tests run: 0, Skipped: $skipped_tests"
        echo "No tests were run. Check --list for available tests."
        exit 0
    fi

    # normal summary
    echo "Tests: $passed_tests/$total_tests passed"
    if [[ $skipped_tests -gt 0 ]]; then
        echo "Skipped: $skipped_tests"
    fi

    if [[ $failed_tests -eq 0 ]]; then
        if [[ $total_tests -gt 0 ]]; then
            echo "All tests passed!"
        fi
        exit 0
    else
        echo "Failed: $failed_tests"
        echo "Check logs in: $log_dir"
        exit 1
    fi
}

main "$@"
