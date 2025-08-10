use bindgen::callbacks::{DeriveInfo, ParseCallbacks};
use cmake::Config;
use std::env;
use std::path::PathBuf;

#[derive(Debug)]
struct MacroCallback {}

impl ParseCallbacks for MacroCallback {
    fn add_derives(&self, info: &DeriveInfo<'_>) -> Vec<String> {
        let mut der = vec![];
        let ser = vec![
            "Color",
            "BaseElement",
            "BasePlant",
            "BaseAnimal",
            "Element",
            "Plant",
            "Animal",
            "Product",
            "Ingredient",
            "Property",
            "Scroll",
        ];
        let co_cl = vec![
            "InventoryElement",
            "Element",
            "Plant",
            "Animal",
            "Ingredient",
            "Product",
            "Scroll",
        ];
        if ser.contains(&info.name) {
            der.append(&mut vec![String::from("serde::Serialize")]);
        }
        if co_cl.contains(&info.name) {
            der.append(&mut vec![String::from("Copy, Clone")]);
        }
        // if info.name == "Test" {
        //     vec!["PartialEq".into()]
        // } else if info.name == "MyOrderedEnum" {
        //     vec!["std::cmp::PartialOrd".into()]
        // } else if info.name == "TestDeriveOnAlias" {
        //     vec!["std::cmp::PartialEq".into(), "std::cmp::PartialOrd".into()]
        // } else {
        // vec![]
        // }
        der
    }
}

fn main() {
    println!("cargo:rerun-if-changed=../core");
    println!("cargo:rerun-if-changed=cpp-src");
    let server_cpp = Config::new("cpp-src").build_target("server_cpp").build();

    println!(
        "cargo:rustc-link-search=native={}/build/",
        server_cpp.display()
    );
    println!(
        "cargo:rustc-link-search=native={}/build/core",
        server_cpp.display()
    );
    println!("cargo:rustc-link-lib=dylib=pime");
    println!("cargo:rustc-link-lib=dylib=server_cpp");

    let bindings = bindgen::Builder::default()
        .header("cpp-src/headers_wrapper.h")
        .clang_arg("-xc++")
        .clang_arg("-std=c++14")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks::new()))
        .parse_callbacks(Box::new(MacroCallback {}))
        .generate()
        .expect("Unable to generate bindings");

    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap()).join("core_bindings.rs");
    bindings
        .write_to_file(out_path)
        .expect("Couldn't write bindings!");
}
