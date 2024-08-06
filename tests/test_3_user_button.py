import pytest
import hardpy
from hardpy.pytest_hardpy.utils.dialog_box import (
    DialogBoxWidget,
    DialogBoxWidgetType,
    DialogBox,
)

from dut_driver import DutDriver

pytestmark = [
    pytest.mark.module_name("User interface"),
    pytest.mark.dependency("test_1_fw::test_fw_flash"),
]


@pytest.mark.case_name("User button")
def test_user_button(device_under_test: DutDriver):
    hardpy.set_message(f"Push the user button")
    keystroke = device_under_test.req_button_press()
    dbx = DialogBox(
        dialog_text=(
            f"Enter the number of times the button has "
            f"been pressed and press the Confirm button"
        ),
        title_bar="User button",
        widget=DialogBoxWidget(DialogBoxWidgetType.NUMERIC_INPUT),
    )
    user_input = int(hardpy.run_dialog_box(dbx))
    assert keystroke == user_input, (
        f"The DUT counted {keystroke} keystrokes"
        f"and the user entered {user_input} keystrokes"
    )
