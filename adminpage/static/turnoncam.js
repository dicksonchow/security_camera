var video = document.querySelector('#video_frame');

navigator.getUserMedia = navigator.getUserMedia || navigator.webkitGetUserMedia
    || navigator.mozGetUserMedia || navigator.msGetUserMedia || navigator.oGetUserMedia;

if (navigator.getUserMedia) {
    navigator.getUserMedia({video: true}, onVideoHandling, onVideoError);
}

function onVideoHandling(stream) {
    video.src = window.URL.createObjectURL(stream);
}

function onVideoError(e) {
    // empty function
}