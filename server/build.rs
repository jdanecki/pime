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
            "Player",
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
        .allowlist_type("Player")
        .allowlist_file(".*packet_types.h")
        .allowlist_type("BaseElementServer")
        .allowlist_type("BasePlant")
        .allowlist_type("BaseAnimal")
        .allowlist_type("PlayerServer")
        .allowlist_type("Element")
        .allowlist_type("Ingredient")
        .allowlist_type("Product")
        .allowlist_type("Plant")
        .allowlist_type("Animal")
        .allowlist_type("Scroll")
        .allowlist_item("WORLD_SIZE")
        .allowlist_item("TILE_SIZE")
        .allowlist_item("CHUNK_SIZE")
        .allowlist_item("TICK_DELAY")
        .allowlist_var("world_table")
        .allowlist_var("objects_to_create")
        .allowlist_var("objects_to_update")
        .allowlist_function("update_location")
        .allowlist_function("time")
        .allowlist_function("srand")
        .allowlist_function("rand")
        .allowlist_function("craft_entry")
        .allowlist_function("update")
        .allowlist_function("add_object_to_world")
        .allowlist_function("find_in_world")
        .allowlist_function("create_element")
        .allowlist_function("create_scroll")
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
