Dependencies:
- nlohmann/json

For e-ink display:
1.) get the lgpio Library
	wget https://github.com/joan2937/lg/archive/master.zip
	unzip master.zip
	cd lg-master
	make
	sudo make install
 
2.) get the official Waveshare Code:
	git clone https://github.com/waveshareteam/e-Paper.git
	cd e-Paper/RaspberryPi_JetsonNano/c
	make

For the weather:
    Source: https://github.com/bundesAPI/dwd-api/blob/main/README.md
    sudo apt-get install libcurl4-openssl-dev

Run the display:
	cd ~/e-Paper/RaspberryPi_JetsonNano/c
	make clean
	make EPD=epd7in5V2
	sudo./epd
