const input = document.querySelector('input');
const info = document.getElementById('info');
const results = document.getElementById('results');

if (window.location.protocol == 'file:') {
    info.innerHTML = "Search is not available when browsing the documentation locally using file:///. \
                      Please use an HTTP server as described in \
                      <a href=\"https://hdoc.io/docs/intro/getting-started/#viewing-the-results\">hdoc's online documentation</a>";
    throw new Error("Search is unsupported when browsing documentation locally.");
}

var miniSearch;
var worker = new Worker('worker.js');
worker.onmessage = function(e) {
    var options = {
        idField: 'sid',
        fields: ['name', 'decl'], // fields to index for full-text search
        storeFields: ['decl', 'type', 'sid'] // fields to return with search results
    };
    miniSearch = MiniSearch.loadJSON(e.data, options);
    worker.terminate()

    // Reveal input and display message once loading is complete
    input.style.display = "block";
    info.innerText = 'Loading index complete.';
}

function typeIntToStr(typeInt) {
    switch(typeInt) {
    case 0:
        return "method";
    case 1:
        return "function";
    case 2:
        return "struct";
    case 3:
        return "class";
    case 4:
        return "union";
    case 5:
        return "enum";
    case 6:
        return "enum val";
    default:
        return "";
    }
}

function updateSearchResults() {
    info.innerText = '';

    if (input.value.length < 3) {
        results.style.display = "none";
        info.innerText = 'Input too short.';
        results.innerHTML = '';
        return;
    }

    const searchOptions = {
        prefix: true,
        fuzzy: 0.2,
        boost: { name: 2}
    }
    const res = miniSearch.search(input.value, searchOptions).slice(0, 90);

    // Only needed for the first call of USR after indexing because
    // otherwise an ugly grey line will appear for the empty results table
    results.style.display = "block";
    results.innerHTML = '';

    res.forEach(function(obj){
        var template =
            '<a class="list-item is-family-code" ';
        // Method
        if (obj.type === 0) {
            template += 'href="r' + obj.id + '">';
        }
        // Function
        if (obj.type === 1) {
            template += 'href="f' + obj.id + '.html">';
        }
        // Class, struct, or union
        if (obj.type === 2 || obj.type === 3 || obj.type === 4) {
            template += 'href="r' + obj.id + '.html">';
        }
        // Enum or enum val
        if (obj.type === 5 || obj.type === 6) {
            template += 'href="e' + obj.id + '.html">';
        }
        // Test comment
        template += '<span class="tag is-dark is-family-sans-serif">' + typeIntToStr(obj.type) + '</span>';
        template += '<strong> ' + obj.decl + '</strong></a>';
        results.innerHTML += template;
    });
}
