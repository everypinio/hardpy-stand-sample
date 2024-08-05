import pytest
from hardpy import (
    CouchdbLoader,
    CouchdbConfig,
    get_current_report,
)

from dut_driver import DutDriver


@pytest.fixture(scope="session")
def device_under_test():
    dut = DutDriver("/dev/ttyACM0", 115200)
    yield dut


def finish_executing():
    report = get_current_report()
    if report:
        loader = CouchdbLoader(CouchdbConfig())
        loader.load(report)


@pytest.fixture(scope="session", autouse=True)
def fill_actions_after_test(post_run_functions: list):
    post_run_functions.append(finish_executing)
    yield
