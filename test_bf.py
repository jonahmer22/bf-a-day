import os
import re
import subprocess
import pytest

ROOT = os.path.dirname(__file__)
TESTS = os.path.join(ROOT, "tests")


def day_dirs():
    entries = []
    for name in sorted(os.listdir(ROOT)):
        if re.fullmatch(r"day\d+", name):
            entries.append(os.path.join(ROOT, name))
    return entries


def build(day_dir):
    result = subprocess.run(["make"], cwd=day_dir, capture_output=True)
    assert result.returncode == 0, f"make failed in {day_dir}:\n{result.stderr.decode()}"
    return os.path.join(day_dir, "bf")


def run(binary, program, stdin=b"", timeout=10):
    if isinstance(program, str) and os.path.isfile(program):
        args = [binary, program]
        input_data = stdin
    else:
        # write inline program to a temp file
        import tempfile
        with tempfile.NamedTemporaryFile(suffix=".b", delete=False, mode="w") as f:
            f.write(program)
            tmp = f.name
        args = [binary, tmp]
        input_data = stdin

    result = subprocess.run(args, input=input_data, capture_output=True, timeout=timeout)

    if "tmp" in locals():
        os.unlink(tmp)

    return result


@pytest.fixture(scope="module", params=day_dirs(), ids=lambda p: os.path.basename(p))
def binary(request):
    return build(request.param)


def test_hello_world(binary):
    r = run(binary, os.path.join(TESTS, "hello_world.b"))
    assert r.stdout == b"Hello World!\n"


def test_array_size(binary):
    program = (
        "++++[>++++++<-]>[>+++++>+++++++<<-]>>++++<[[>[[>>+<<-]<]>>>-]>-[>+>+<<-]>]"
        "+++++[>+++++++<<++>-]>.<<."
    )
    r = run(binary, program)
    assert r.stdout == b"#\n"


def test_obscure_problems(binary):
    program = (
        '[]++++++++++[>>+>+>++++++[<<+<+++>>>-]<<<<-]\n'
        '"A*$";?@![#>>+<<]>[>>]<<<<[>++<[-]]>.>.'
    )
    r = run(binary, program)
    assert r.stdout == b"H\n"


def test_io_newline(binary):
    program = ">,>+++++++++,>+++++++++++[<++++++<++++++<+>>>-]<<.>.<<-.>.>.<<."
    r = run(binary, program, stdin=b"\n")
    lines = r.stdout.splitlines()
    assert len(lines) == 2, f"expected 2 lines, got: {r.stdout!r}"
    assert lines[0] == lines[1], f"lines differ: {lines}"
    assert re.fullmatch(rb"[A-Z]{2}", lines[0]), f"unexpected content: {lines[0]!r}"


def test_rot13(binary):
    import subprocess
    try:
        r = run(binary, os.path.join(TESTS, "rot13.b"), stdin=b"~mlk zyx", timeout=5)
        out = r.stdout
    except subprocess.TimeoutExpired as e:
        out = e.stdout or b""
    assert out[:8] == b"~zyx mlk", f"expected '~zyx mlk' at start of output, got: {out[:20]!r}"


def test_unmatched_open(binary):
    r = run(binary, os.path.join(TESTS, "unmatched_open.b"))
    assert r.stdout == b"", f"expected no output, got: {r.stdout!r}"
    assert r.returncode != 0 or r.stderr, "expected error indication for unmatched ["


def test_unmatched_close(binary):
    r = run(binary, os.path.join(TESTS, "unmatched_close.b"))
    assert r.stdout == b"", f"expected no output, got: {r.stdout!r}"
    assert r.returncode != 0 or r.stderr, "expected error indication for unmatched ]"
