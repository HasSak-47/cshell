from .clangd import init_clang

def main():
    clangd = init_clang()


    clangd.terminate()

if __name__ == "__main__":
    main()
