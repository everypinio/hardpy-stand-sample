import pytest
import hardpy

from hardpy import (
    NumericInputWidget,
    DialogBox,
)


from dut_driver import DutDriver


pytestmark = [
    pytest.mark.module_name("Base functions"),
    pytest.mark.dependency("test_1_fw::test_fw_flash"),
]


@pytest.mark.case_name("DUT info")
def test_serial_num(device_under_test: DutDriver):
    serial_number = device_under_test.reqserial_num()
    hardpy.set_dut_serial_number(serial_number)
    assert serial_number == "test_dut_1"

    info = {
        "name": serial_number,
        "batch": "batch_1",
    }
    hardpy.set_dut_info(info)


@pytest.mark.case_name("LED")
def test_jumper_closed(device_under_test: DutDriver):
    hardpy.set_message("Сount the number of LED blinks")
    jumper_status = device_under_test.req_jumper_status()
    assert jumper_status == 0, "LED does not light up, check the jumper"

    dbx = DialogBox(
        dialog_text="Enter the number of LED blinks",
        title_bar="LED check",
        widget=NumericInputWidget(),
    )
    user_input = int(hardpy.run_dialog_box(dbx))
    assert 10 == user_input, f"The LED blinked 10 times, user entered {user_input}"
