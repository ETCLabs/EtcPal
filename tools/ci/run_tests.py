"""Convert the unit test output to JUnit XML."""

import argparse
import junit_xml
import os
import subprocess
import sys
import unity_test_parser


BUILD_DIR = os.path.join(os.path.dirname(os.path.realpath(__file__)), "..", "..", "build")

def main():
    """The script entry point."""

    # Parse the command-line arguments.
    parser = argparse.ArgumentParser(description="Run EtcPal unit tests and produce JUnit XML.")
    parser.add_argument("build_dir", help="The CMake build directory")
    parser.add_argument("--config", help="The CMake build configuration to run tests for")
    args = parser.parse_args()

    test_exe_dir = os.path.join(args.build_dir, "tests", "test_executables")
    if args.config:
        test_exe_dir = os.path.join(test_exe_dir, args.config)

    failed = False

    for file_name in os.listdir(test_exe_dir):
        # Get the executable name of the test
        with open(os.path.join(test_exe_dir, file_name), "r") as test_exe_file:
            test_exe_name = test_exe_file.read()
        test_name = file_name.replace(".txt", "")

        # Run the test
        print("############# Running test '{}'... #############".format(test_name))
        process_result = subprocess.run([test_exe_name, "-v"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

        # Interpret the output
        if process_result.returncode != 0:
            failed = True
        result_output = process_result.stdout.decode("utf-8")
        print(result_output)
        results = unity_test_parser.TestResults(result_output, unity_test_parser.UNITY_FIXTURE_VERBOSE)

        with open(os.path.join(args.build_dir, "test_results_{}.xml".format(test_name)), "w") as output_file:
            junit_xml.TestSuite.to_file(output_file, [results.to_junit()])

    if failed:
        sys.exit(1)

if __name__ == "__main__":
    main()
