use std::env;
use std::path::PathBuf;

fn main() {
    println!("cargo:rustc-link-lib=libmtc_decoder");

    // let bindings = bindgen::Builder::default()
    //     .header("mtccdecoder.h")
    //     .header("tmtcdecoder.h")
    //     .clang_arg("-I./include")
    //     .generate()
    //     .expect("Unable to generate bindings");
    //
    // bindings
    //     .write_to_file(PathBuf::from("bindings.rs"))
    //     .expect("Couldn't write bindings!");
}
