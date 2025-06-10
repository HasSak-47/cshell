use std::{
    fmt::format,
    fs::{self, File},
    io::Write,
};

use clang::{Clang, EntityKind, Index, get_version};

#[derive(Debug, Default, Clone)]
enum LuaType {
    #[default]
    Nil,
    Int,
    IntEnum,
    Boolean,
    Number,
    Str,
    UserData(String),
}

fn match_types(ty: &str) -> LuaType {
    match ty {
        "int" => LuaType::Int,
        "bool" => LuaType::Boolean,
        "_Bool" => LuaType::Boolean,
        "float" => LuaType::Number,
        "void" => LuaType::Nil,
        _ if ty.contains("char *") => LuaType::Str,
        _ if ty.contains("enum") => LuaType::IntEnum,
        _ => LuaType::UserData(ty.to_string()),
    }
}

fn make_bind(name: &str, params: Vec<(&str, LuaType)>, com: String) -> String {
    let line = com
        .lines()
        .filter(|line| line.find("@luabind").is_some())
        .next()
        .unwrap()
        .to_string();
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
    let index = Index::new(&clang, true, true);

    writeln!(out_file, "version: {}", get_version()).unwrap();
    for entry in fs::read_dir("./hot").unwrap() {
        let entry = entry.unwrap();
        let path = entry.path();
        out_file
            .write(format!("file: {}\n", path.display()).as_bytes())
            .unwrap();

        let tu = index
            .parser(path)
            .arguments(&[
                "-H",
                "-Iinclude",
                "-Wall",
                "-Werror",
                "-llua",
                "-I/usr/include",
                "-I/usr/lib/clang/19/include",
                "-Iinclude/",
            ])
            .parse()
            .unwrap();

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

            let ret = function.get_result_type().unwrap();
            let r_ty = ret.get_display_name();
            let r_canon_ty = ret.get_canonical_type().get_display_name();
            let r_type_name = match_types(&r_canon_ty);

            writeln!(
                out_file,
                "{f_name} -> {r_ty} aka {r_canon_ty} therefore lua.{r_type_name:?}"
            )
            .unwrap();

            for child in def {
                let name = child.get_name().unwrap();
                let ty = child.get_type().unwrap().get_display_name();
                let canon_ty = child
                    .get_type()
                    .unwrap()
                    .get_canonical_type()
                    .get_display_name();

                let type_name = match_types(&canon_ty);
                writeln!(out_file, "{name}: {canon_ty} therefore lua.{type_name:?}").unwrap();
            }
            out_file.write(b"\n").unwrap();
        }
    }
}
