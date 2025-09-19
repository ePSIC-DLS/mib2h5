#!/usr/bin/env bash
set -euo pipefail

# constants
readonly script_dir="$(dirname "$(readlink -f "$0")")"
readonly project_root="$(cd "$script_dir/../.." && pwd)"
readonly python_dir="$project_root/python"
readonly log_dir="$script_dir/logs"

# version requirements
readonly min_python_major=3
readonly min_python_minor=10

total_tests=0
passed_tests=0
failed_tests=0
skipped_tests=0
test_filter="all"

cleanup() {
    cd "$project_root" 2>/dev/null || true
    if [[ -f Makefile ]]; then
        make distclean >/dev/null 2>&1 || true
    fi

    # clean python build artefacts
    rm -rf "$python_dir/build" 2>/dev/null || true
    rm -rf "$python_dir/dist" 2>/dev/null || true
    rm -rf "$python_dir/src/mib2h5.egg-info" 2>/dev/null || true
    rm -rf "$python_dir/src/mib2h5/_wrapper.c" 2>/dev/null || true
    rm -rf "$python_dir/src/mib2h5/_wrapper."*.so 2>/dev/null || true
    rm -rf "$python_dir/__pycache__" 2>/dev/null || true
    rm -rf "$python_dir/src/mib2h5/__pycache__" 2>/dev/null || true

    # clean pipx installation
    if command -v pipx >/dev/null 2>&1; then
        pipx uninstall mib2h5 >/dev/null 2>&1 || true
    fi
}

# register cleanup on exit
trap cleanup EXIT

check_prerequisites() {
    local errors=0

    # check hdf5_root is set
    if [[ -z "${HDF5_ROOT:-}" ]]; then
        echo "ERROR: HDF5_ROOT environment variable not set"
        errors=$((errors + 1))
    elif [[ ! -d "${HDF5_ROOT}" ]]; then
        echo "ERROR: HDF5_ROOT directory does not exist: ${HDF5_ROOT}"
        errors=$((errors + 1))
    fi

    # check python version
    if ! command -v python3 >/dev/null 2>&1; then
        echo "ERROR: python3 not found"
        errors=$((errors + 1))
    else
        local python_version
        python_version=$(python3 -c 'import sys; print(f"{sys.version_info.major}.{sys.version_info.minor}")')
        local major minor
        IFS='.' read -r major minor <<< "$python_version"
        if [[ "$major" -lt $min_python_major ]] || [[ "$major" -eq $min_python_major && "$minor" -lt $min_python_minor ]]; then
            echo "ERROR: Python $min_python_major.$min_python_minor+ required, found $python_version"
            errors=$((errors + 1))
        fi
    fi

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
    local install_method="$2"

    total_tests=$((total_tests + 1))
    echo -n "Test: $test_name... "

    # run in subshell for complete isolation
    if (
        # clear environment variables that might interfere
        unset PYTHONPATH
        unset PIP_CONFIG_FILE
        unset PIP_CACHE_DIR

        # set required environment
        export LD_LIBRARY_PATH="${HDF5_ROOT}/lib:${LD_LIBRARY_PATH:-}"

        # run the actual test
        run_test_internal "$test_name" "$install_method"
    ); then
        passed_tests=$((passed_tests + 1))
    else
        failed_tests=$((failed_tests + 1))
    fi
}

# actual test runner
run_test_internal() {
    local test_name="$1"
    local install_method="$2"
    local log_file="${log_dir}/${test_name}.log"

    # create temp directory for virtual environment
    local temp_dir
    temp_dir=$(mktemp -d /tmp/mib2h5_test_XXXXXX)
    local venv_dir="$temp_dir/venv"

    # ensure temp directory cleanup
    trap "rm -rf '$temp_dir'" RETURN

    # clean previous build
    cleanup

    # create virtual environment
    if ! python3 -m venv "$venv_dir" >>"$log_file" 2>&1; then
        echo "FAIL (venv creation)"
        return 1
    fi

    # use venv's pip directly (no need to activate in subshell)
    local pip_cmd="$venv_dir/bin/pip"
    local python_cmd="$venv_dir/bin/python"

    # upgrade pip and install build dependencies
    if ! "$pip_cmd" install --upgrade pip 'setuptools>=77.0' wheel 'cython>=3' >>"$log_file" 2>&1; then
        echo "FAIL (pip dependencies)"
        return 1
    fi

    # change to python directory
    cd "$python_dir"

    # install package based on method
    case "$install_method" in
        "standard")
            if ! "$pip_cmd" install . >>"$log_file" 2>&1; then
                local error_line
                error_line=$(grep -E "error:|ERROR:|FAILED" "$log_file" | tail -1 | head -c 60)
                if [[ -n "$error_line" ]]; then
                    echo "FAIL (install: $error_line...)"
                else
                    echo "FAIL (install)"
                fi
                return 1
            fi
            ;;
        "editable")
            if ! "$pip_cmd" install -e . >>"$log_file" 2>&1; then
                local error_line
                error_line=$(grep -E "error:|ERROR:|FAILED" "$log_file" | tail -1 | head -c 60)
                if [[ -n "$error_line" ]]; then
                    echo "FAIL (editable install: $error_line...)"
                else
                    echo "FAIL (editable install)"
                fi
                return 1
            fi
            ;;
        "pipx")
            # for pipx, we use the global pipx command
            export PIPX_DEFAULT_PYTHON=$(which python3)
            if ! pipx install . --verbose >>"$log_file" 2>&1; then
                local error_line
                error_line=$(grep -E "error:|ERROR:|FAILED" "$log_file" | tail -1 | head -c 60)
                if [[ -n "$error_line" ]]; then
                    echo "FAIL (pipx install: $error_line...)"
                else
                    echo "FAIL (pipx install)"
                fi
                return 1
            fi
            ;;
        *)
            echo "FAIL (unknown install method)"
            return 1
            ;;
    esac

    # test cli version
    local mib2h5_cmd
    if [[ "$install_method" == "pipx" ]]; then
        mib2h5_cmd="mib2h5"
    else
        mib2h5_cmd="$venv_dir/bin/mib2h5"
    fi

    if ! "$mib2h5_cmd" --version >>"$log_file" 2>&1; then
        echo "FAIL (--version)"
        return 1
    fi

    # test cli help
    if ! "$mib2h5_cmd" --help >>"$log_file" 2>&1; then
        echo "FAIL (--help)"
        return 1
    fi

    # test python import (skip for pipx as it uses isolated environment)
    if [[ "$install_method" != "pipx" ]]; then
        if ! "$python_cmd" -c "import mib2h5; print('Import successful')" >>"$log_file" 2>&1; then
            echo "FAIL (import)"
            return 1
        fi
    fi

    # cleanup pipx installation
    if [[ "$install_method" == "pipx" ]]; then
        pipx uninstall mib2h5 >>"$log_file" 2>&1 || true
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
            echo "Require setting HDF5_ROOT to the HDF5 installation"
            echo
            echo "Available tests:"
            echo "  standard  - Standard pip install"
            echo "  editable  - Editable/development install"
            echo "  pipx      - pipx install"
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

    echo "=== Python Build Tests ==="
    if [[ "$test_filter" != "all" ]]; then
        echo "Running only: $test_filter"
    fi
    echo "HDF5_ROOT: ${HDF5_ROOT:-not set}"
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

    # test 1: standard install
    if should_run_test "standard" "$test_filter"; then
        run_isolated_test "standard_install" "standard"
    fi

    # test 2: editable install
    if should_run_test "editable" "$test_filter"; then
        run_isolated_test "editable_install" "editable"
    fi

    # test 3: pipx install
    if should_run_test "pipx" "$test_filter"; then
        if command -v pipx >/dev/null 2>&1; then
            run_isolated_test "pipx_install" "pipx"
        else
            skipped_tests=$((skipped_tests + 1))
            echo "Test: pipx_install... SKIP (pipx not found)"
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
