set -e -x

if ! command -v yasm &> /dev/null || [[ $(yasm --version) != *"1.3.0"* ]]; then
    # yasm on CentOS 5 is too old, install a newer version
    # Version 1.3.0 is the latest stable release.
    curl -SLO http://www.tortall.net/projects/yasm/releases/yasm-1.3.0.tar.gz
    tar -xvf yasm-1.3.0.tar.gz
    cd yasm-1.3.0
    ./configure --prefix=/usr/local
    make
    make install
    cd ..
fi

# CMake is also too old. Modern C++ projects, especially with TensorFlow,
# require a much newer version. Let's update to a more recent one (e.g., 3.16.3).
CMAKE_VERSION="3.16.3"
curl -SLO "https://github.com/Kitware/CMake/releases/download/v${CMAKE_VERSION}/cmake-${CMAKE_VERSION}.tar.gz"
tar -xvf "cmake-${CMAKE_VERSION}.tar.gz"
cd "cmake-${CMAKE_VERSION}"
./configure --prefix="/usr/local/cmake-${CMAKE_VERSION}"
make
make install
export PATH="/usr/local/cmake-${CMAKE_VERSION}/bin:$PATH"
cd ..

function lex_pyver {
     # Echoes Python version string padded with zeros
     # Thus:
     # 3.2.1 -> 003002001
     # 3     -> 003000000
     echo $1 | awk -F "." '{printf "%03d%03d%03d", $1, $2, $3}'
}


for PYBIN in /opt/python/*/bin; do

# Patch python-config scripts (https://github.com/pypa/manylinux/pull/87)
# Remove -lpython from the python-config script.

    if [ -e ${PYBIN}/python3 ]; then
        ln -sf python3 ${PYBIN}/python
        ln -sf python3-config ${PYBIN}/python-config
    fi

    py_ver="$(${PYBIN}/python -c 'import platform; print(platform.python_version())')"

    if [ $(lex_pyver $py_ver) -lt $(lex_pyver 3.4) ]; then
        echo "Patching python 2"
        sed -i "s/'-lpython' *+ *pyver\( *+ *sys.abiflags\)\?/''/" $(readlink -e ${PYBIN}/python-config)
    else
        echo "Patching python 3"
        sed -i 's/-lpython${VERSION}${ABIFLAGS}//' $(readlink -e ${PYBIN}/python-config)
    fi
done
