from glob import glob
from pathlib import Path

import hardpy
import pytest

from pyocd.core.helpers import ConnectHelper
from pyocd.flash.file_programmer import FileProgrammer


pytestmark = pytest.mark.module_name("Testing preparation")


def firmware_find():
    fw_dir = Path(Path(__file__).parent.resolve() / "dut-nucleo.hex")
    return glob(str(fw_dir))


@pytest.mark.case_name("Availability of firmware")
def test_fw_exist():
    assert firmware_find() != []


@pytest.mark.case_name("Check DUT connection")
def test_check_connection(device_under_test):
    assert device_under_test.serial is not None, "DUT not connected"


@pytest.mark.dependency("test_1_fw::test_check_connection")
@pytest.mark.case_name("Flashing firmware")
def test_fw_flash():
    fw = firmware_find()[0]
    hardpy.set_message(f"Flashing file {fw}...", "flash_status")

    with ConnectHelper.session_with_chosen_probe(
        target_override="stm32f401retx"
    ) as session:
        # Load firmware into device.
        FileProgrammer(session).program(fw)

    hardpy.set_message(f"Successfully flashed file {fw}", "flash_status")
    assert True
