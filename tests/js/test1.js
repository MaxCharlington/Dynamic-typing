function* numbers() {
    let i = 0;
    const f = () => i++;
    yield* [f(), f(), f()].map(i => i + 1);
}

const arr = [];

for (let x of numbers()) {
    arr.push(x);
}

IO.write_to_stdout(arr.join(","));
