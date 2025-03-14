use std::str::Lines;


#[derive(Debug)]
enum Literal{
    U32(u32),
    I32(i32),
    F32(f32),
    String(String),
}

#[derive(Debug)]
enum Token {
    Literal(Literal),

    /* [\d\s_]+ */
    Identifier(String),
    Operator(String),
    Keyword(String),
    Undefined(String),
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
            .chars()
            .into_iter();
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

        return new;
    }

    pub fn token_stream<S: AsRef<str>>(s: S) -> Vec<Self>{
        let mut lines : Vec<_> = s.as_ref().to_string().lines().map(ToString::to_string).collect();
        let mut s = String::new();
        for line in &mut lines{
            *line = line.trim_matches(char::is_whitespace).to_string();
            let matches : Vec<_> = line.match_indices("--").map(|m| m.0).collect();
            for mch in matches {
                if mch >= line.len(){
                    break;
                }
                *line = line[0..mch].to_string();
            }
            s.push_str(line.as_str());
            s.push(' ');
        }


        let mut iter = s.chars().enumerate();
        let mut token : Vec<Token> = vec![];
        let mut current = String::new();
        loop {
            let (idx, curr) = match iter.next(){
                Some(c) => c,
                None => break,
            };
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
                            if first{
                                first = false;
                            }

                            if (double && c == '"') || (!double && c == '\'') { break; }
                            else{ string.push(c); }
                        }
                    }

                }
                println!("string: \"{string}\"");
                token.push(Token::Literal(Literal::String(string)));
            }

            if curr.is_whitespace(){

                current = String::new();
            }
            else{
                current.push(curr);
            }
        }


        return token;
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

#[test]
fn name() {
    const S : &str = include_str!("../init.lua");
    let stream = Token::token_stream(S);
    for token in stream{
        // println!("{token:?}");
    }

}
