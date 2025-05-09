use std::{
    fs::{self, File},
    io::Write,
};

use clang::{Clang, EntityKind, Index};

fn make_bind(name: &str, params: Vec<(&str, &str)>, com: String) -> String {
    let line = com
        .lines()
        .filter(|line| line.find("@luabind").is_some())
        .next()
        .unwrap()
        .to_string();

    let bind_name = format!("api_bind_{name}");
    for (name, ty) in params{
    }


    let mut s = String::new();
    return s;
}

fn main() {
    let mut out_file = File::create("testing.txt").unwrap();

    for entry in fs::read_dir("hot").unwrap() {
        let path = entry.unwrap().path();
        let f = format!("cargo:rerun-if-changed={}", path.display());
        out_file.write_all(f.as_bytes()).unwrap();
        out_file.write("\n".as_bytes()).unwrap();

        println!("{f}");
    }
    println!("cargo:rerun-if-changed=.");
    println!("cargo:rerun-if-changed=hot/");

    let clang = Clang::new().unwrap();
    let index = Index::new(&clang, false, false);

    for entry in fs::read_dir("./hot").unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        out_file
            .write(format!("file: {}\n", path.display()).as_bytes())
            .unwrap();

        let tu = index.parser(path).parse().unwrap();

        let functions: Vec<_> = tu
            .get_entity()
            .get_children()
            .into_iter()
            .filter(|e| {
                if e.get_kind() != EntityKind::FunctionDecl {
                    return false;
                }

                if let Some(s) = e.get_comment() {
                    if !s.contains("@luabind") {
                        return false;
                    }
                } else {
                    return false;
                }

                return true;
            })
            .collect();

        for function in functions {
            let f_name = function.get_name().unwrap();
            let def = function.get_arguments().unwrap();
            let com = function.get_comment().unwrap();

            out_file.write(format!("{f_name}\n").as_bytes()).unwrap();
            for child in def {
                let name = child.get_name().unwrap();
                let ty = child.get_type().unwrap().get_display_name();
                out_file
                    .write(format!("{name}: {ty}\n").as_bytes())
                    .unwrap();
            }
            out_file.write(b"\n").unwrap();
        }
    }
}
