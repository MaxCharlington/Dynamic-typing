const { exit } = require('process');
const { parse } = require('abstract-syntax-tree')
const fs = require('fs');

const args = process.argv.slice(2);
const path_to_js = args[0];
const path_to_out_json = args[1];

let source;
try
{
    source = fs.readFileSync(path_to_js, 'utf8');
}
catch (err)
{
    console.error(err);
    exit(1);
}

const tree = parse(source);

try
{
    fs.writeFileSync(path_to_out_json, JSON.stringify(tree, null, 4));
}
catch (err)
{
    console.error(err);
    exit(1);
}
