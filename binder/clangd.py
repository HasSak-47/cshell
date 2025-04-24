import re
from typing import Dict, List
from .lsp import Lsp, lsp_write_message, lsp_read_message

def init_clang():
    args = ['clangd', '--compile-commands-dir=.', '--log=error']
    clangd = Lsp(args)
    if clangd.stdin == None or clangd.stdout == None:
        exit(-1)

    lsp_write_message(clangd,{
        'jsonrpc': '2.0',
        'id': 1,
        'method': 'initialize',
        'params': {
            'processId': None,
            'rootUri': None,
            'capabilities': {},
        }
    })

    # Read Ok message
    lsp_read_message(clangd)

    return clangd

def __extract_name_and_return(text: str) :
    lines = text.strip().splitlines()

    name: str | None = None
    return_type: str | None = None

    for line in lines:
        if name is None:
            name = line
        elif line.startswith("→") or line.startswith("=>"):
            return_type = line.split(maxsplit=1)[1]
            return_type = re.sub(r'\(.*\)', '', return_type).strip(' ')
            break  # Done

    if name is None:
        raise RuntimeError('could not resolve name')


    return ( name, return_type )

def __parse_function_doc(lines: List[str]):
    args = []
    desc = ''
    
    for line in lines[:-1]:
        if line.startswith('-'):
            arg_line = line[2:]
            arg_line = re.sub(r'\(.*\)', '', arg_line)
            match = re.match(r'^(.*)\s([a-zA-Z_][a-zA-Z0-9_]*)$', arg_line)
            if match is None:
                continue
            ty = match.group(1)
            name = match.group(2)
            args.append((name, ty))
        else:
            desc += line
    return (args, desc)


# func, params, desc
def parse_func_sign(value: str|None):
    if value is None:
        return
    ps = value.split('Parameters:\n')
    if ps == 1:
        return 

    (func, *params) = ps
    func = func.replace('function ', '')
    while '  ' in func:
        func = func.replace('  ', ' ')
    while '\n\n' in func:
        func = func.replace('\n\n', '\n')

    func = __extract_name_and_return(func)
    desc = ''
    if len(params) > 0:
        lines = list(filter(lambda x: len(x) > 0, str(params[0]).splitlines()))
        params, desc = __parse_function_doc(lines)

    return (func, params, desc)

    
