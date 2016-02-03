var video = document.getElementById("video_frame");
var canvas = document.getElementById("picframe");
var ctx = canvas.getContext("2d");

var imgArr = new Array();
var formData = new FormData();
var pic_count = 2;

function dataURItoBlob(dataURI) {

    // convert base64 to raw binary data held in a string
    // doesn't handle URLEncoded DataURIs
    var byteString;
    if (dataURI.split(',')[0].indexOf('base64') >= 0)
        byteString = atob(dataURI.split(',')[1]);
    else
        byteString = unescape(dataURI.split(',')[1]);

    // separate out the mime component
    var mimeString = dataURI.split(',')[0].split(':')[1].split(';')[0]

    // write the bytes of the string to an ArrayBuffer
    var ab = new ArrayBuffer(byteString.length);
    var ia = new Uint8Array(ab);
    for (var i = 0; i < byteString.length; i++) {
        ia[i] = byteString.charCodeAt(i);
    }

    //Passing an ArrayBuffer to the Blob constructor appears to be deprecated,
    //so convert ArrayBuffer to DataView
    var dataView = new DataView(ab);
    var blob = new Blob([dataView], {type: mimeString});

    return blob;
}

function upload_to_server() {
    console.log($("#login_username").val())
    formData.append("user", $("#login_username").val())
    $.ajax({
        type: 'POST',
        url: '/upload',
        data: formData,
        processData: false,
        contentType: false,
        success: function () {
            alert("Successfully upload files to server");
            window.location.replace("/home");
        }
    });
}

$("#snapshot").click(function () {
    canvas.width = video.videoWidth;
    canvas.height = video.videoHeight;
    ctx.drawImage(video, 0, 0, canvas.width, canvas.height);
    $("#taking_picture").slideUp();
    $("#to_confirm").slideDown();
});

$("#discard").click(function () {
    ctx.clearRect(0, 0, canvas.width, canvas.height);
    $("#to_confirm").slideUp();
    $("#taking_picture").slideDown();
});

$("#save").click(function () {
    formData.append("file", dataURItoBlob(canvas.toDataURL("image/jpeg")), "tmp_" + pic_count + ".jpg");
    pic_count--;

    if (pic_count == 0) {
        $("#login-modal").modal("show");
    }
    else {
        $("#pic_left").text(pic_count - imgArr.length)
        $("#to_confirm").slideUp();
        $("#taking_picture").slideDown();
    }
});
