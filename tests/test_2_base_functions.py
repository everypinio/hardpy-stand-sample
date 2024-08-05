import pytest
import hardpy

from dut_driver import DutDriver

pytestmark = pytest.mark.module_name("Base functions")


@pytest.mark.case_name("DUT info")
def test_serial_num(device_under_test: DutDriver):
    serial_number = device_under_test.req_serial_num()
    hardpy.set_dut_serial_number(serial_number)
    assert serial_number == "test_dut_1"

    info = {
        "name": "test_dut_1",
        "batch": "batch_1",
    }
    hardpy.set_dut_info(info)

@pytest.mark.case_name("LED")
def test_jumper_closed(device_under_test: DutDriver):
    assert device_under_test.req_jumper_status() == 0
