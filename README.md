# HardPy stand sample

This is an example of using [HardPy](https://github.com/everypinio/hardpy) to create a functional test stand.

> Warning! To use this example, you will need python 3.10 or higher and docker to run CouchDB 3.2 or higher.

## Repository structure

#### tests

Test stand using HardPy.

#### database

Config for database deployment.

#### dut_fw

DUT firmware source code. 
Can be modified if desired. 
The assembled firmware is in tests directory.

## Stand preparation

1. Clone repository.
1. Install `python` environment:
    ```bash
    $ python -m venv env
    $ source env/bin/activate
    $ pip install -r tests/requirements.txt
    ```
1. Install `pyocd` in the current environment:
    ```bash
    $ pyocd pack update
    $ pyocd pack install stm32f401retx
    ```

## Run CouchDB

1. Install docker.
1. Run database `sudo`:
    
    ```bash
    docker run --name couchdb -p 5984:5984 -e COUCHDB_USER=dev -e COUCHDB_PASSWORD=dev -v ./database/couchdb.ini:/opt/couchdb/etc/local.ini couchdb:3.3
    ```

    or from `database` directory:

    ```bash
    docker compose up
    ```
1. The database is available at http://127.0.0.1:5984/_utils
Login/password: `dev`

## HardPy operator panel

Run `hardpy-panel tests` and check browser:

http://localhost:8000/

## Stand description

The Nucleo F401RE devboard is used as the DUT.

Tests:

- Checking the availability of firmware for the board;
- Flashing the devboard;
- Read serial number;
- Check if there is a jumper on the pins (see photo below).
If the jumper is present, the LED flashes, the test passes. 
If there is no jumper, the test fails;
- Read the number of clicks on the User Button and display it in the operator panel interface;

<img src="docs/nucleo.jpg" width=600><br>
