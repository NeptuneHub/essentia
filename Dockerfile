# STAGE 1: Builder
# This stage compiles Essentia for the target architecture.
FROM ubuntu:22.04 AS builder

# Set non-interactive frontend
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    ca-certificates \
    git \
    wget \
    python3-pip \
    pkg-config \
    libeigen3-dev \
    libfftw3-dev \
    libavcodec-dev \
    libavformat-dev \
    libavutil-dev \
    libswresample-dev \
    libchromaprint-dev \
    libyaml-dev \
    libyaml-cpp-dev \
    libtag1-dev \
    libsamplerate0-dev \
    libatlas-base-dev \
    python3-dev \
    && rm -rf /var/lib/apt/lists/*

# Install Python dependencies for the build
RUN pip3 install --no-cache-dir numpy==1.26.4

# Install TensorFlow based on the target architecture
ARG TARGETARCH
RUN if [ "$TARGETARCH" = "arm64" ]; then \
        pip3 install --no-cache-dir tensorflow-aarch64==2.16.1; \
    else \
        pip3 install --no-cache-dir tensorflow==2.16.1; \
    fi

# Clone your Essentia repository
WORKDIR /
RUN git clone https://github.com/NeptuneHub/essentia.git
WORKDIR /essentia

# Apply the NumPy C-API compatibility fix
# This is the crucial step to fix the build error.
RUN sed -i 's/PyArrayObject/PyArrayObject/g' src/python/pytypes/*.cpp && \
    sed -i 's/->data/PyArray_DATA/g' src/python/pytypes/*.cpp && \
    sed -i 's/->strides/PyArray_STRIDES/g' src/python/pytypes/*.cpp && \
    sed -i 's/->descr/PyArray_DESCR/g' src/python/pytypes/*.cpp && \
    sed -i 's/->nd/PyArray_NDIM/g' src/python/pytypes/*.cpp && \
    sed -i 's/->dimensions/PyArray_DIMS/g' src/python/pytypes/*.cpp

# Setup TensorFlow
RUN python3 src/3rdparty/tensorflow/setup_tensorflow.py --mode python

# Configure, build, and install Essentia
RUN python3 ./waf configure --build-static --with-python --with-tensorflow
RUN python3 ./waf
RUN python3 ./waf install

# --- End of builder stage ---

# STAGE 2: Final application image
FROM ubuntu:22.04

ENV LANG=C.UTF-8 \
    PYTHONUNBUFFERED=1 \
    DEBIAN_FRONTEND=noninteractive

WORKDIR /app

# Install runtime dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    python3 python3-pip \
    libfftw3-3 libyaml-0-2 libtag1v5 libsamplerate0 \
    ffmpeg \
    && rm -rf /var/lib/apt/lists/*

# Install Python runtime packages
RUN pip3 install --no-cache-dir numpy==1.26.4

# Install TensorFlow runtime based on architecture
ARG TARGETARCH
RUN if [ "$TARGETARCH" = "arm64" ]; then \
        pip3 install --no-cache-dir tensorflow-aarch64==2.16.1; \
    else \
        pip3 install --no-cache-dir tensorflow==2.16.1; \
    fi

# Copy the pre-built essentia Python package from the builder stage
COPY --from=builder /usr/local/lib/python3.10/dist-packages/essentia* /usr/local/lib/python3.10/dist-packages/

# Copy the required TensorFlow runtime libraries from the builder stage
# and update the linker's cache.
COPY --from=builder /usr/local/lib/libtensorflow* /usr/local/lib/
COPY --from=builder /usr/local/lib/_pywrap_tensorflow_internal.so /usr/local/lib/
RUN ldconfig

# Set the PYTHONPATH to include the installed packages
ENV PYTHONPATH=/usr/local/lib/python3.10/dist-packages:/app

# Your application entrypoint
# This is a placeholder for your application's command.
CMD ["python3"]