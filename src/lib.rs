use std::{fmt::Display, str::Lines};


#[derive(Debug)]
enum Literal{
    U32(u32),
    I32(i32),
    F32(f32),
    String(String),
}

impl Display for Literal {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self{
            Self::U32(a) => write!(f, "{a}"),
            Self::I32(a) => write!(f, "{a}"),
            Self::F32(a) => write!(f, "{a}"),
            Self::String(a) => write!(f, "\"{a}\""),
        }
    }
}

#[derive(Debug, Clone, Copy)]
#[repr(u32)]
enum Delimiter{
    OpenPar  = '(' as u32,
    ClosePar = ')' as u32,
    OpenSBr  = '[' as u32,
    CloseSBr = ']' as u32,
    OpenCBr  = '{' as u32,
    CloseCBr = '}' as u32,
    Coma     = ',' as u32,
    Dot      = '.' as u32,
}

impl Display for Delimiter{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        unsafe{
            write!(f, "{}", char::from_u32_unchecked(*self as u32))
        }
    }
}

#[derive(Debug)]
enum Token {
    Literal(Literal),

    /* [\d\s_]+ */
    Delimiter(Delimiter),
    Identifier(String),
    Operator(String),
    Keyword(String),
    Undefined(String),
    NewLine,
}

impl Display for Token{
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self{
            Self::NewLine       => write!(f, "\n"),
            Self::Delimiter(a)  => write!(f, "{a}"),
            Self::Literal(a)    => write!(f, "{a}"),
            Self::Identifier(a) => write!(f, "{a}"),
            Self::Operator(a)   => write!(f, "{a}"),
            Self::Keyword(a)    => write!(f, "{a}"),
            Self::Undefined(a)  => write!(f, "{a}"),
        }
        
    }
}

impl Token{
    fn needs_expand(&self) -> bool {
        match &self{
            Self::Undefined(_) => { true },
            _ => { false }
        }
    }
    fn expand(self) -> Vec<Self> {
        if !self.needs_expand(){ return vec![self]; }
        let mut new = vec![];
        let mut string = match self{
            Self::Undefined(s) => s,
            _ => unreachable!(),
        };

        let mut iter = string
            .chars();
        let mut splits = vec![0usize];
        loop {
            let next = iter.position(|c| !(c.is_ascii_alphanumeric() || c == '_'));
            if next.is_none() {
                break;
            }
            let next = next.unwrap();
            splits.push(next);
        }
        splits.push(string.len());
        splits.sort();

        let mut identifiers = vec![];
        let mut toggle = false;
        for i in 0..(splits.len() -1) {
            let curr = splits[i];
            let next = splits[i + 1];
            let sub = string[curr..next].to_string();
            identifiers.push(
                if toggle { Token::Identifier(sub) }
                else{ Token::Operator(sub) }
            );
            toggle = !toggle;
        }

        new
    }

    pub fn process_line<S: AsRef<str>>(s: S) -> Vec<Self> {
        let rf = s.as_ref().replace(";", "\n");
        let mut iter = rf.chars().enumerate().peekable();
        let mut current = String::new();
        let mut token : Vec<Token> = vec![];

        current = current.replace("\n", "");
        'line: while let Some((_idx, curr)) = iter.next() {
            if curr == '-' {
                if let Some((_idx_n, curr_n)) = iter.peek(){
                    // comment
                    if *curr_n == '-'{
                        token.push(Token::Undefined(current));
                        token.push(Token::NewLine);
                        return token;
                    }
                }
            }
            // inside quotes
            if curr == '"' || curr == '\''{
                let double = curr == '"';

                if !current.is_empty() {
                    token.push(Token::Undefined(current));

                    current = String::new();
                }

                let mut string = String::new();
                let mut first = true;
                loop {
                    let curr = iter.next();
                    match curr{
                        None => break,
                        Some((_, c)) =>{
                            if c == '\n'{
                                continue 'line;
                            }
                            if first{
                                first = false;
                            }

                            if (double && c == '"') || (!double && c == '\'') { break; }
                            else{ string.push(c); }
                        }
                    }

                }
                token.push(Token::Literal(Literal::String(string)));
                continue;
            }
            current.push(curr);
        }
        token.push(Token::Undefined(current));
        token.push(Token::NewLine);

        return token;

    }

    pub fn expand_whitespace(mut self) -> Vec<Self> {
        if let Self::Undefined(mut s) = self {
            s = s.replace("\t", " ");
            while s.contains("  "){
                s = s.replace("  ", " ");
            }

            let mut tokens = vec![];
            let mut curr = String::new();
            for letter in s.trim().chars(){
                if letter == ' '{
                    tokens.push(Self::Undefined(curr));
                    curr = String::new();
                    continue;
                }
                curr.push(letter);
            }
            if !curr.is_empty(){
                tokens.push(Token::Undefined(curr));
            }

            return tokens;
        }
        return vec![self];
    }

    // TODO: rename this one
    pub fn ya_expansion(mut self) -> Vec<Self>{
        if let Self::Undefined(s) = self{
            let mut indicies = Vec::new();
            for del in DELIMITERS{
                if let Some(i) = s.find(del){
                    indicies.push(i);
                }
            }

            return vec![];
        }
        return vec![self];
    }

    pub fn token_stream<S: AsRef<str>>(s: S) -> Vec<Self>{
        let lines = s.as_ref().lines();
        let mut tokens = vec![];
        for line in lines{
            let mut v = Self::process_line(line);
            tokens.append(&mut v);
        }
        let mut tks = Vec::new();
        for token in tokens{
            let mut other = token.expand_whitespace();
            tks.append(&mut other);
        }

        return tks;

    }
}

const KEYWORDS: &[&str] = &[
    "if",
    "else",
    "elseif",
    "end",

    "and",
    "or",

    "true",
    "false",
    "while",
    "for",
    "in",
    "do",
    "break",
    "function",
    "goto",
    "local",
    "nil",
    "not",
    "repeat",
    "return",
    "then",
    "until",
];

const OPERATORS: &[&str] = &[
    "=",
    "+",
    "-",
    "*",
    "/",
    "..", // concat
];

const DELIMITERS: &[&str] = &[
    "(", ")",
    "[", "]",
    "{", "}",
    ",", ".",
];

#[test]
fn name() {
    const S : &str = include_str!("../init.lua");
    let stream = Token::token_stream(S);
    for token in stream{
        // println!("{token:?}");
    }

}
