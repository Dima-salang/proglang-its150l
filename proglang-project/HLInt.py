
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
        try:
            tokens = self.scan_file(filename)
            ast = self.parse(tokens)
            print(ast)
        except Exception as e:
            print(e)

    def scan_file(self, filename):
        with open(filename, "r") as f:
            self.clean_source(f.readlines())
            scanner = Scanner(filename)
            scanner.scan()
            scanner.print_tokens()
            return scanner.tokens

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

    def parse(self, tokens):
        parser = Parser(tokens)
        return parser.parse()

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
        self.has_error = False

        # indentation
        self.indent_level = 0
        self.indent_stack = [0]
        self.is_at_start_line = True
    
    def scan_file(self, filename):
        with open(filename, "r") as f:
            self.source = f.read()
        
    def scan(self):
        self.scan_file(self.source)
        while not self.is_at_end():
            # handle the indentation problem
            if self.is_at_start_line:
                self.handle_indentation()
            
            if not self.is_at_end():
                self.start_idx = self.current_idx
                self.scan_token()

        self.add_token("EOF")

    def handle_indentation(self):
        current_indent = 0
        self.is_at_start_line = False
        while self.next_char() == " ":
            self.advance_char()
            current_indent += 1

        last_indent = self.indent_stack[-1]

        if self.next_char() == "\n":
            self.is_at_start_line = True
            return


        # we indent
        if current_indent > last_indent:
            self.indent_stack.append(current_indent)
            self.tokens.append(Token("INDENT", "", None, self.line))
        # there is a dedent that occured
        elif current_indent < last_indent:
            while self.indent_stack[-1] > current_indent:
                self.indent_stack.pop()
                self.tokens.append(Token("DEDENT", "", None, self.line))

            if self.indent_stack[-1] != current_indent:
                self.error("Indentation error")
        

    def scan_token(self):
        # since we are scanning tokens, we are not at the start of line
        self.is_at_start_line = False

        if self.next_char() == " ":
            # do nothing since it is just whitespace
            self.advance_char()
            return
            
        character = self.advance_char()
        match character:
            case "+":
                self.add_token("PLUS")
            case "-":
                self.add_token("MINUS")
            case "*":
                self.add_token("STAR")
            case "/":
                self.add_token("SLASH")
            case "!":
                self.add_token("BANG_EQUAL" if self.is_next("=") else "BANG")
            case "<":
                if self.is_next("="):
                    self.add_token("LESS_EQUAL")
                elif self.is_next("<"):
                    self.add_token("PRINT_OP")
                else:
                    self.add_token("LESS")
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
                self.is_at_start_line = True
            case '"':
                self.add_string()
            case _:
                if character.isdigit():
                    self.add_number()
                elif character.isalpha():
                    self.add_identifier()
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

    def add_identifier(self):
        while self.next_char().isalpha() and not self.is_at_end():
            self.advance_char()
        text = self.source[self.start_idx:self.current_idx]
        if text in RESERVED_WORDS:
            self.add_token(RESERVED_WORDS[text])
        else:
            self.add_token("IDENTIFIER", text)

    def next_char(self, offset=0):
        if self.is_at_end(): return "\0"
        return self.source[self.current_idx + offset]

    def print_tokens(self):
        for token in self.tokens:
            print(token)


    def error(self, message):
        print(message)
        self.has_error = True
    
            

    


        
        
        
class Token:
    def __init__(self, type, lexeme, literal, line):
        self.type = type
        self.lexeme = lexeme
        self.literal = literal
        self.line = line

    def __str__(self):
        return f"{self.type} {self.lexeme} {self.literal} {self.line}"



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


RESERVED_WORDS = {
    "if": "IF",
    "output": "OUTPUT",
}
    

# Parser

class Parser:
    def __init__(self, tokens):
        self.tokens = tokens
        self.current_idx = 0
        self.has_error = False

    def parse(self):
        try:
            return self.expression()
        except Exception as e:
            print(e)
        
    


    # define the nonterminals 
    def expression(self):
        print("expression")
        return self.equality()

    def equality(self):
        print("equality")
        expr = self.comparison()
        while self.match("EQUAL_EQUAL", "BANG_EQUAL"):
            operator = self.previous_token()
            right = self.comparison()
            expr = Binary(expr, operator, right)
            print(f"equality: {expr}")
        return expr

    def comparison(self):
        print("comparison")
        expr = self.term()
        while self.match("GREATER", "GREATER_EQUAL", "LESS", "LESS_EQUAL"):
            operator = self.previous_token()
            right = self.term()
            expr = Binary(expr, operator, right)
            print(f"comparison: {expr}")
        print(f"comparison: {expr}")
        return expr

    def term(self):
        print("term")
        expr = self.factor()
        while self.match("PLUS", "MINUS"):
            operator = self.previous_token()
            right = self.factor()
            expr = Binary(expr, operator, right)
            print(f"term: {expr}")
        return expr

    def factor(self):
        print("factor")
        expr = self.unary()
        while self.match("STAR", "SLASH"):
            operator = self.previous_token()
            right = self.unary()
            expr = Binary(expr, operator, right)
            print(f"factor: {expr}")
        return expr

    def unary(self):
        print("unary")
        if self.match("BANG", "MINUS"):
            operator = self.previous_token()
            right = self.unary()
            return Unary(operator, right)
        return self.primary()

    def primary(self):
        print("primary")
        if self.match("FALSE"): return Literal(False)
        if self.match("TRUE"): return Literal(True)
        if self.match("NUMBER", "STRING"):
            literal = Literal(self.previous_token().literal)
            print(literal)
            return literal
        if self.match("LEFT_PAREN"):
            expr = self.expression()
            self.consume("RIGHT_PAREN", "Expect right parenthesis after expression")
            grouping = Grouping(expr)
            print(grouping)
            return grouping
        self.error(self.next_token(), "Expect expression")

    def advance_token(self):
        if self.is_at_end(): return None
        self.current_idx += 1
        return self.previous_token()


    def match(self, *types):
        if self.is_at_end():
            return False
        if self.next_token().type in types:
            self.advance_token()
            return True
        return False

    def check_type(self, type):
        if self.is_at_end(): return False
        if self.next_token().type == type:
            return True
        return False
    
    def is_at_end(self):
        if self.current_idx >= len(self.tokens):
            return True
        return self.tokens[self.current_idx].type == "EOF"

    def next_token(self, offset=0):
        if self.is_at_end(): return None
        token = self.tokens[self.current_idx + offset]
        return token

    def previous_token(self, offset=1):
        if self.is_at_end(): return None
        token = self.tokens[self.current_idx - offset]
        return token

    def consume(self, type, message):
        if self.check_type(type):
            return self.advance_token()
        self.error(self.next_token(), message)

    def error(self, token, message):
        if token.type == "EOF":
            print(f"{token.line} at end: {message}")
        else:
            print(f"{token.line} {token.lexeme}: {message}")
        self.has_error = True



# AST Nodes
class Expr:
    """  Base class   """
    pass


class Binary(Expr):
    def __init__(self, left, operator, right):
        self.left = left
        self.operator = operator
        self.right = right
    
    def __str__(self):
        return f"({self.left} {self.operator} {self.right})"

class Unary(Expr):
    def __init__(self, operator, right):
        self.operator = operator
        self.right = right
    
    def __str__(self):
        return f"({self.operator} {self.right})"


class Grouping(Expr):
    def __init__(self, expression):
        self.expression = expression
    
    def __str__(self):
        return f"({self.expression})"

class Literal(Expr):
    def __init__(self, value):
        self.value = value
    
    def __str__(self):
        return f"{self.value}"

def main():
    HLInt(sys.argv).run()


if __name__ == "__main__":
    main()
