
# command line program
import sys

class HLInt:
    error = False

    def __init__(self, args):
        self.args = args

    def run(self):
        if (len(self.args) == 1):
            print("Usage: python HLInt.py <filename>")
            sys.exit(1)
        elif (len(self.args) > 2):
            print("Usage: python HLInt.py <filename>")
            sys.exit(1)
        else:
            self.run_file(self.args[1])

    def run_file(self, filename):
        self.scan_file(filename)

    def scan_file(self, filename):
        with open(filename, "r") as f:
            self.clean_source(f.readlines())
            scanner = Scanner(f.read())
            scanner.scan()

    def clean_source(self, source):
        # remove newlines and spaces
        cleaned_lines = []

        for line in source:
            indent = len(line) - len(line.lstrip(" \t"))
            stripped = line.strip()

            if not stripped:
                continue

            parts = stripped.split()
            cleaned_content = " ".join(parts)
            
            cleaned_line = (" " * indent) + cleaned_content
            cleaned_lines.append(cleaned_line)

        # write the new source to a file
        with open("NOSPACES.txt", "w") as f:
            f.writelines(cleaned_lines)

    def scan_source(self, filename):
        tokens = []

    def error(self, message):
        print(message)
        self.error = True

class Scanner:
    def __init__(self, source):
        self.source = source
        self.tokens = []
        self.start_idx = 0
        self.current_idx = 0
        self.line = 1
        
    def scan(self):
        while not self.is_at_end():
            self.start_idx = self.current_idx
            self.scan_token()

    def scan_token(self):
        character = self.advance_char()
        match character:
            case "+":
                self.add_token("PLUS")
            case "-":
                self.add_token("MINUS")
            case "!":
                self.add_token("BANG_EQUAL" if self.is_next("=") else "BANG")
            case "<":
                self.add_token("LESS_EQUAL" if self.is_next("=") else "LESS")
            case ">":
                self.add_token("GREATER_EQUAL" if self.is_next("=") else "GREATER")
            case "=":
                self.add_token("EQUAL_EQUAL" if self.is_next("=") else "EQUAL")
            case ":":
                self.add_token("COLON_EQUAL" if self.is_next("=") else "COLON")
            case "(":
                self.add_token("LEFT_PAREN")
            case ")":
                self.add_token("RIGHT_PAREN")
            case ";":
                self.add_token("SEMICOLON")
            case "\n":
                self.line += 1
                self.add_token("NEWLINE")
            case "\t":
                self.add_token("INDENT")
            case "\r":
                self.add_token("DEDENT")
            case '"':
                self.add_string()
            case _:
                if character.isdigit():
                    self.add_number()
                else:
                    self.error(f"Unexpected character: {character}")

    def is_next(self, expected):
        if self.is_at_end(): return False
        if (self.source[self.current_idx] != expected): return False
        self.current_idx += 1
        return True

    
    def advance_char(self):
        self.current_idx += 1
        return self.source[self.current_idx - 1]

        

    def is_at_end(self):
        return self.current_idx >= len(self.source)

    
    def add_token(self, type, value=None):
        text = self.source[self.start_idx:self.current_idx]
        self.tokens.append(Token(type, text, value, self.line))

    def add_string(self):
        while self.next_char() != '"' and not self.is_at_end():
            if self.next_char() == '\n':
                self.line += 1
            self.advance_char()

        if self.is_at_end():
            self.error("Unterminated string")
        self.advance_char()
        unquoted = self.source[self.start_idx+1:self.current_idx-1]
        self.add_token("STRING", unquoted)

    def add_number(self):
        # scan for the entire integer part
        while self.next_char().isdigit() and not self.is_at_end():
            self.advance_char()

        # scan for the entire fractional part
        if (self.next_char() == "." and self.next_char(1).isdigit()):
            self.advance_char()
            while self.next_char().isdigit() and not self.is_at_end():
                self.advance_char()
        self.add_token("NUMBER", float(self.source[self.start_idx:self.current_idx]))



    def next_char(self, offset=0):
        if self.is_at_end(): return "\0"
        return self.source[self.current_idx + offset]
    


        
        
        
class Token:
    def __init__(self, type, lexeme, literal, line):
        self.type = type
        self.lexeme = lexeme
        self.literal = literal
        self.line = line



TOKEN_TYPES = [
    "IDENTIFIER",
    "NUMBER",
    "PLUS",
    "MINUS",
    "EQUAL",
    "COLON",
    "EQUAL_EQUAL",
    "NOT_EQUAL",
    "GREATER",
    "GREATER_EQUAL",
    "LESS",
    "LESS_EQUAL",
    "NEWLINE",
    "INDENT",
    "DEDENT",
    "PRINT_OP"
]

    
            




def main():
    HLInt(sys.argv).run()


if __name__ == "__main__":
    main()
