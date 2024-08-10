document.addEventListener('DOMContentLoaded', function() {
    var selectElement = document.getElementById("resourceList");

    // Faire une requête pour récupérer la liste des fichiers
    var xhr = new XMLHttpRequest();
    xhr.open('GET', '/listFiles', true);  // Assurez-vous que cette URL retourne la liste des fichiers en JSON

    xhr.onreadystatechange = function() {
        if (xhr.readyState === XMLHttpRequest.DONE) {
            if (xhr.status === 200) {
                var files = JSON.parse(xhr.responseText);
                files.forEach(function(file) {
                    var option = document.createElement("option");
                    option.value = file;
                    option.textContent = file;
                    selectElement.appendChild(option);
                });
            } else {
                alert("Failed to load files.");
            }
        }
    };

    xhr.send();
});
