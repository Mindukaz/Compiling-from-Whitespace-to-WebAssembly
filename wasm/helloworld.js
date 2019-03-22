
    var importObject = {
        console:
                {   read_char: read_char_f,
                    read_num: read_num_f,
                    write_char: write_char_f,
                    write_num: write_num_f,
                }
    };

    function read_char_f() {
        var input = prompt("Enter a character:");
        var input = readline();
        while (input.length != 1) {
            input = prompt("Enter a SINGLE CHARACTER:");
        }
        return input.charCodeAt(0);
    }

    function read_num_f() {
        var input = Number(prompt("Enter an integer value:"));
        while ( !Number.isInteger(input) ) {
            input = Number(prompt("Enter an INTEGER value:"));
        }
        return input;
    }

    function write_char_f(param) {
        console.log( String.fromCharCode(param) );
    }

    function write_num_f(param) {
        console.log(param);
    }


    // loading wasm
    var main;
    function loadWebAssembly(fileName, imports) {
      return fetch(fileName)
        .then(response => response.arrayBuffer())
        .then(bits => WebAssembly.compile(bits))
        .then(module => { return new WebAssembly.Instance(module, imports) });
    };

    loadWebAssembly("helloworld.wasm", importObject)
      .then(instance => {
        main = instance.exports.main;
        console.log('Finished compiling! Type main() to run the program');
    });
    