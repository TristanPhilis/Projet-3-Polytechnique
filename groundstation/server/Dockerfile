FROM ubuntu:20.04
RUN apt update
RUN apt install -y python3-pip udev
RUN echo \
'# Crazyradio (normal operation)\
SUBSYSTEM=="usb", ATTRS{idVendor}=="1915", ATTRS{idProduct}=="7777", MODE="0664", GROUP="plugdev"\
# Bootloader\
SUBSYSTEM=="usb", ATTRS{idVendor}=="1915", ATTRS{idProduct}=="0101", MODE="0664", GROUP="plugdev"'\
> /etc/udev/rules.d/99-crazyflie.rules
COPY ./requirements.txt ./requirements.txt
RUN pip install -r requirements.txt
WORKDIR /app
COPY . .
ARG SIM_ARG
ENV ENV_SIM_ARG ${SIM_ARG}
EXPOSE 5000
EXPOSE 8080
CMD python3 ./app.py $ENV_SIM_ARG
