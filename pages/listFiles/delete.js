function sendDeleteRequest(url) {
    fetch(url, {
        method: 'DELETE',
        headers: {
            'Content-Type': 'application/json'
        }
    })
    .then(response => {
        if (response.ok) {
            return response.text();
        }
        throw new Error('Network response was not ok.');
    })
    .then(text => {
        console.log('Success:', text);
        alert('File deleted successfully');
        window.location.reload();
    })
    .catch(error => {
        console.error('Error:', error);
        alert('Failed to delete file');
    });
}

document.addEventListener('DOMContentLoaded', () => {
    const deleteButtons = document.querySelectorAll('.delete-button');
    deleteButtons.forEach(button => {
        button.addEventListener('click', (event) => {
            const url = event.target.getAttribute('data-url');
            if (url) {
                sendDeleteRequest(url);
            }
        });
    });
});