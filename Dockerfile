FROM gcc:bullseye
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update && apt-get install -y
# RUN apt build-dep libsdl2
# RUN apt install libdrm-dev libgbm-dev
# RUN wget -c https://libsdl.org/release/SDL2-2.26.2.tar.gz
# RUN tar -zxf SDL2-2.26.2.tar.gz --directory ~/sdl-src
# RUN cd ~/sdl-src && \
#     ./configure --enable-video-kmsdrm && \
#     make -j4 && \
    # make install
RUN apt-get install -y libsdl2-image-dev libglm-dev libglew-dev libopengl-dev cmake