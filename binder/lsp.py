from subprocess import Popen, PIPE
from typing import Dict
import json

class Lsp:
    def __init__(self, args) -> None:
        self.proc = Popen(args, stdin=PIPE, stdout=PIPE)

        assert self.proc.stdin is not None 
        assert self.proc.stdout is not None

        self.stdin = self.proc.stdin
        self.stdout = self.proc.stdout

    def __del__(self) -> None:
        self.proc.terminate()

def lsp_write_message(lsp:Lsp, msg: Dict):

    body = json.dumps(msg)
    header = f'Content-Length: {len(body)}\r\n\r\n'
    lsp.stdin.write(bytes(header + body, encoding='utf-8'))
    lsp.stdin.flush()

def lsp_read_message(lsp: Lsp):
    assert lsp.stdout is not None

    headers = {}
    while True:
        line : str = lsp.stdout.readline().decode('utf-8')
        if line == '\r\n' or line == '\n':
            break
        key, value = line.strip().split(':', 1)
        headers[key.strip()] = value.strip()

    content_length = int(headers.get('Content-Length', 0))
    content = lsp.stdout.read(content_length)

    return json.loads(content)

def open_document(lsp:Lsp, name: str):
    with open(name, 'r') as file:
        text = file.read()
        uri = f'file:///{name}'
        lsp_write_message(lsp, {
            'jsonrpc': '2.0',
            'method': 'textDocument/didOpen',
            'params': {
                'textDocument': {
                    'uri': uri,
                    'languageId': 'c',
                    'version': 1,
                    'text': text
                }
            }
        })

    return lsp_read_message(lsp)

def get_all_symbols(lsp:Lsp, f: None | str=None):
    FILTERS = {
	    'File' : 1,
	    'Module' : 2,
	    'Namespace' : 3,
	    'Package' : 4,
	    'Class' : 5,
	    'Method' : 6,
	    'Property' : 7,
	    'Field' : 8,
	    'Constructor' : 9,
	    'Enum' : 10,
	    'Interface' : 11,
	    'Function' : 12,
	    'Variable' : 13,
	    'Constant' : 14,
	    'String' : 15,
	    'Number' : 16,
	    'Boolean' : 17,
	    'Array' : 18,
	    'Object' : 19,
	    'Key' : 20,
	    'Null' : 21,
	    'EnumMember' : 22,
	    'Struct' : 23,
	    'Event' : 24,
	    'Operator' : 25,
	    'TypeParameter' : 26,
    }

    lsp_write_message(lsp, {
        'jsonrpc': '2.0',
        'id': 1,
        'method' : 'workspace/symbol',
        'params' :{ 'query': '', }
    })

    syms = lsp_read_message(lsp)
    if f != None and f in FILTERS:
        syms['result'] = list(filter(lambda x: x['kind'] == FILTERS[f], syms['result']))
    
    return syms['result']

def get_function_signature(lsp: Lsp):
    pass
