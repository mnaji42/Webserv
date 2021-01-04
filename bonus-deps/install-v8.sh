
v8_full_compile() {
    cd /tmp
    git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
    export PATH=/tmp/depot_tools:$PATH
    cd ~
    gclient
    fetch v8
    cd v8
    tools/dev/v8gen.py x64.release.sample
    ninja -C out.gn/x64.release.sample v8_monolith
}

v8_precompiled() { 
    cd ~
    wget https://github.com/cchudant/tmp-archive/raw/v8/v8.tar.gz
    tar -xvzf v8.tar.gz
    rm v8.tar.gz
}

v8_precompiled
