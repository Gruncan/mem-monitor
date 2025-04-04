use std::{env, fs};
use std::path::PathBuf;
use std::process::Command;

const RUST_C_BUILD_OUTPUT_DIR_DEFAULT: &str = "src";


fn main() {
    println!("cargo:rerun-if-changed=src/mtccdecoder.c");
    println!("cargo:rerun-if-changed=src/tmtcdecoder.c");
    println!("cargo:rerun-if-changed=Cargo.lock");
    println!("cargo:rerun-if-changed=include");


    let out_dir = PathBuf::from(env::var("RUST_C_BUILD_OUTPUT_DIR").as_deref()
                                                    .unwrap_or(RUST_C_BUILD_OUTPUT_DIR_DEFAULT));

    let mut objects = Vec::new();

    for source_file in ["src/mtccdecoder.c", "src/tmtcdecoder.c"] {
        let source_string = String::from(source_file);
        let obj_file = compile_c_file(source_string, &out_dir);
        objects.push(obj_file);
    }

    create_static_library(&objects, &out_dir);

    println!("cargo:rustc-link-search=native={}", out_dir.display());
    println!("cargo:rustc-link-lib=static=mtc_decoder");

    println!("\n==================================");
    println!("\nCreating bindings...\n");
    println!("==================================\n");
    let bindings = bindgen::Builder::default()
        .header("include/mtccdecoder.h")
        .header("include/tmtcdecoder.h")
        .header("../config/mem-monitor-config.h")
        .clang_arg("-Iinclude")
        .clang_arg("-I../config")
        .generate()
        .expect("Unable to generate bindings");

    let bindings_path = out_dir.join("bindings.rs");

    bindings
        .write_to_file(&bindings_path)
        .expect("Couldn't write bindings!");

    println!("\n==================================");
    println!("\nUpdating bindings...\n");
    println!("==================================\n");
    let bindings_content = fs::read_to_string(&bindings_path).expect("Failed to read bindings.rs");
    let modified_content = format!("#![allow(warnings)]\n{}", bindings_content);

    fs::write(&bindings_path, modified_content).expect("Failed to write modified bindings.rs");

}

fn compile_c_file(source: String, out_dir: &PathBuf) -> PathBuf {
    let filename = String::from(PathBuf::from(source.as_str()).file_stem().unwrap().to_str().unwrap());
    let obj_path = out_dir.join(format!("{}.o", filename));

    let status = Command::new("gcc")
        .args([
            "-Wall",
            "-Wextra",
            "-O3",
            "-std=c17",
            "-Iinclude",
            "-I../config",
            "-c",
            source.as_str(),
            "-o",
            obj_path.to_str().unwrap()
        ])
        .status()
        .expect(&format!("Failed to compile {}", source));

    if !status.success() {
        panic!("Compilation of {} failed", source);
    }

    obj_path
}

fn create_static_library(objects: &[PathBuf], out_dir: &PathBuf) {
    let lib_path = out_dir.join("libmtc_decoder.a");

    let mut cmd = Command::new("ar");
    cmd.args(["rcs", lib_path.to_str().unwrap()]);

    for obj in objects {
        cmd.arg(obj.to_str().unwrap());
    }

    let status = cmd.status()
        .expect("Failed to create static library");

    if !status.success() {
        panic!("Creation of static library failed");
    }
}