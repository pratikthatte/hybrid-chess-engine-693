import subprocess

def run_engine_and_test(path_to_engine):
    print(f"Starting engine: {path_to_engine}\n")
    
    process = subprocess.Popen(
        [path_to_engine],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True,
        bufsize=1
    )

    def send(cmd):
        print(f">>> {cmd}")
        process.stdin.write(cmd + "\n")
        process.stdin.flush()

    def read_output_until(keyword, timeout=5):
        import time
        start = time.time()
        lines = []
        while True:
            line = process.stdout.readline()
            if line:
                print(f"<<< {line.strip()}")
                lines.append(line.strip())
                if keyword in line:
                    break
            if time.time() - start > timeout:
                print("Timed out waiting for output.")
                break
        return lines

    # UCI handshake
    send("uci")
    read_output_until("uciok")

    send("isready")
    read_output_until("readyok")

    send("ucinewgame")
    #send("position startpos")

    #send("go movetime 300")
    #lines = read_output_until("bestmove")

    send("position fen 7k/8/8/8/8/8/4Q3/5K2 w - - 0 1")

    send("go")
    lines = read_output_until("bestmove")

    # Cleanup
    send("quit")
    process.stdin.close()
    process.stdout.close()
    process.stderr.close()
    process.wait()

    print("\nTest complete.")
    return lines

if __name__ == "__main__":
    run_engine_and_test("./minimax_engine")