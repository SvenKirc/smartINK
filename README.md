# E-Paper Weather Display

## Setup

### Install Dependencies

#### JSON Library

* nlohmann/json

#### e-ink Display (lgpio)

This project depends on `lgpio` for e-ink display support.

```bash
wget https://github.com/joan2937/lg/archive/master.zip
unzip master.zip
cd lg-master
make
sudo make install
```

#### Weather Support

Based on:
https://github.com/bundesAPI/dwd-api/blob/main/README.md

```bash
sudo apt-get install libcurl4-openssl-dev
```

---

## Usage

### Run the Display

```bash
cd ~/e-Paper/RaspberryPi_JetsonNano/c
make clean
make EPD=epd7in5V2
sudo ./epd
```

---

## Acknowledgements

This project makes use of the Waveshare e-Paper example code:

* https://github.com/waveshareteam/e-Paper

Example setup from the original repository:

```bash
git clone https://github.com/waveshareteam/e-Paper.git
cd e-Paper/RaspberryPi_JetsonNano/c
make
```

Refer to the original repository for full setup instructions and licensing details.
