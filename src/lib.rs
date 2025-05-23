use std::{
    ffi::{CStr, c_char, c_int},
    ptr::null,
};

use clap::Parser;

#[derive(Parser, Debug, Default)]
#[command(version, about, long_about = None)]
struct Args {
    #[arg(short, long, default_value_t = false)]
    debug: bool,
    script: Option<String>,
}

#[unsafe(no_mangle)]
unsafe extern "C" fn parse_args(argc: c_int, argv: *const *const c_char) -> *mut Args {
    let mut args = Vec::new();
    unsafe {
        for i in 0..(argc as usize) {
            let arg = CStr::from_ptr(*(argv.add(i)));
            if let Ok(s) = arg.to_str() {
                args.push(s.to_string());
            }
        }
    }

    let args = Args::parse_from(args);
    return Box::into_raw(Box::new(args));
}

#[unsafe(no_mangle)]
unsafe extern "C" fn is_debug(args: *const Args) -> bool {
    if args.is_null() {
        return false;
    }
    unsafe {
        return (&*args).debug;
    }
}

#[unsafe(no_mangle)]
unsafe extern "C" fn get_script(args: *const Args) -> *const c_char {
    if args.is_null() {
        return null();
    }
    unsafe {
        if let Some(s) = &(&*args).script {
            return s.as_ptr() as *const c_char;
        }
    }

    return null();
}

#[unsafe(no_mangle)]
unsafe extern "C" fn free_args(args: *mut Args) {
    if !args.is_null() {
        unsafe {
            drop(Box::from_raw(args));
        }
    }
}
