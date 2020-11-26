FROM gcc

WORKDIR /t2

RUN apt-get update && apt-get upgrade -y
RUN apt-get install nasm -y

CMD ["bash"]