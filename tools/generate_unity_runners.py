"""
Automatically generates individual test runners and a shared main function for
all generated runners within the Unity testing framework.

This functionality is based on the second example within their repository,
which leverages test groups for combining several tests in one runner, and
then combines these test groups within the `RunAllTests` function. There is no
native runner script provided to support this use case, hence the need for a
custom script like this.

See:
    - https://github.com/ThrowTheSwitch/Unity/tree/v2.6.1/examples/example_2
"""

import re
from pathlib import Path

TEST_DIR = Path("test")
RUNNERS_DIR = TEST_DIR / "runners"
RUNNERS_DIR.mkdir(parents=True, exist_ok=True)

GROUP_PATTERN = re.compile(r"^\s*TEST_GROUP\s*\(\s*(\w+)\s*\)", re.MULTILINE)
TEST_PATTERN = re.compile(r"^\s*TEST\s*\(\s*(\w+)\s*,\s*(\w+)\s*\)", re.MULTILINE)


def parse_test_file(path: Path):
    """Parses a test file into its test groups and their test cases."""
    src = path.read_text()
    groups = GROUP_PATTERN.findall(src)
    tests = TEST_PATTERN.findall(src)

    grouped_tests = {}
    for group in groups:
        grouped_tests[group] = []

    for group, case in tests:
        grouped_tests.setdefault(group, []).append(case)

    return grouped_tests


def generate_runner_source(group: str, test_cases: list[str]) -> str:
    """Generates a runner for a single test group."""
    lines = ['#include "unity_fixture.h"', "", f"TEST_GROUP_RUNNER({group}) {{"]
    for case in test_cases:
        lines.append(f"    RUN_TEST_CASE({group}, {case});")
    lines.append("}")
    lines.append("")  # newline at end
    return "\n".join(lines)


def generate_all_tests_source(groups: list[str]) -> str:
    """Generates the main function, combining all test groups."""
    lines = ['#include "unity_fixture.h"', "", "static void RunAllTests(void) {"]
    for group in sorted(groups):
        lines.append(f"    RUN_TEST_GROUP({group});")
    lines.append("}")
    lines.append("")
    lines.append("int main(int argc, const char *argv[]) {")
    lines.append("    return UnityMain(argc, argv, RunAllTests);")
    lines.append("}")
    lines.append("")
    return "\n".join(lines)


def main():
    test_files = sorted(TEST_DIR.glob("test_*.c"))
    all_groups = {}

    print("Scanning test files...")
    for path in test_files:
        parsed = parse_test_file(path)
        for group, cases in parsed.items():
            all_groups[group] = sorted(set(all_groups.get(group, []) + cases))
        print(f"  Found in {path.name}: {', '.join(parsed.keys()) or '(none)'}")

    print("\nGenerating runners...")
    for group, cases in all_groups.items():
        runner_path = RUNNERS_DIR / f"test_{group.lower()}_runner.c"
        src = generate_runner_source(group, cases)
        runner_path.write_text(src)
        print(f"  Wrote {runner_path}")

    print("\nGenerating main...")
    all_tests_path = RUNNERS_DIR / "all_tests.c"
    all_tests_path.write_text(generate_all_tests_source(list(all_groups.keys())))
    print(f"  Wrote {all_tests_path}")

    print("\nDone!")


if __name__ == "__main__":
    main()
