char Style[] PROGMEM  = R"rawliteral(
<style>
body {
background-color: #b4e0b4;
}
h4 {
margin: 2px;
}
h1 {
color:blue;
text-align:center;
}
.container {
margin: auto;
width: 90%;
max-width: 460px;
background-color: #b5eafe;
padding: 0px 10px;
}
@media(min-width:1200px) {
.container {
margin: auto;
width: 45%;
}
}
@media(min-width:768px) and (max-width:1200px) {
.container {
margin: auto;
width: 55%;
}
}
.q {
float: right;
width: 64px;
text-align: right;
}
.l {
background: url('data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAACAAAAAgCAMAAABEpIrGAAAALVBMVEX///8EBwfBwsLw8PAzNjaCg4NTVVUjJiZDRUUUFxdiZGSho6OSk5Pg4eFydHTCjaf3AAAAZElEQVQ4je2NSw7AIAhEBamKn97/uMXEGBvozkWb9C2Zx4xzWykBhFAeYp9gkLyZE0zIMno9n4g19hmdY39scwqVkOXaxph0ZCXQcqxSpgQpONa59wkRDOL93eAXvimwlbPbwwVAegLS1HGfZAAAAABJRU5ErkJggg==') no-repeat left center;
background-size: 1em;
}
.scnnet {
background-color: beige;
padding: 5px;
margin-bottom: 5px;
}
.strnet {
background-color: bisque;
padding: 5px;
}
.inf {
padding: 2px;
}
textarea {
 max-width:200px;
}
table {
width: 95%;
margin: auto;
border - collapse: collapse;
}

p {
margin-block: 0.8em;
}

td {
padding: 2px;
}
td.stbtn {
border: 1px solid #808080;
padding: 2px 10px;
width: 90%;
}
.ti {
width:95%;
}
.btn {
border-radius: 4px;
border: 0;
color: red;
cursor: pointer;
display: inline-block;
margin: 2px;
padding: 5px;
position: relative;
background-color: beige;
box-shadow: 0px 6px 5px -4px rgba(0,0,0,0.52);
border: 0;
}
.btn:hover {
background: #b4e0b4;
}
.btn:active, #btn:focus {
background: #b5eafe;
}
.btn:disabled {
background-color: rgba(100,100,100,0.1);
cursor: not-allowed;
}
.txt {
padding: 5px;
width: 140px;
}
.attn {
color: red;
font-weight: 600;
}

input[type="file"] {
  display:none;
}
/* Additional Style for controller*/
.btncnt {
width: 160px;
height: 160px;
background-color: cadetblue;
float: left;
}

.btnimg {
background-image: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABQAAAAcCAYAAAEW3a/2AAAABGdBTUEAALGPC/xhBQAAAAlwSFlzAAALEwAACxMBAJqcGAAAAAd0SU1FB+AKDQY6HSBov0wAAAAdaVRYdENvbW1lbnQAAAAAAENyZWF0ZWQgd2l0aCBHSU1QZC5lBwAAAylJREFUSMelVEtoE1EUPW8yiRWbmEwarJoiWGmpoAtRxK1QsVJcCYIbQXBTWkuhWi1VEQRXxYoLwUWp9qNuW2xqFoKItEjQ2vRDI01Ra2zz0SST1szEec9FO0kmmXzEuxnee/edc+65dx4A4M7Nm9eghmtsjAEAh6zgACAcDmd2bvf0XE3nc5q8YCgU0MDpQqpBAODZ4CCzWq3Yuh4hAHCru/s64TgKIHjn7t0BHgDq6uru2e12APACGNjEJGTrQzJE3NYmYyyjTV2kBfvm5o4IgqCuD6dPvB8/HlSFp3WqMfTkCbNnboEQ4j3d3Hw4v8ocPlVhXiIhGgLQrIvaRC6HgNLMmdvlcqysrHwwGo0KbzDsUz0FgN/JZGo9kVhVFCWa5lr2+Y5PTk1N2Wy2dGIwFPp+8dIlp163MO3x1D/t71/VbSMAPOzru/wzErGxTbcZpRT3ent7/0gS1SS+e/PmsyiKB9SW8DyPxqYmHoCiqZoxRnSU5PtIckzMa3Yhs3PXpaj1O6NrR5mIrKRGlsvgdrkmw6HQXpPJtKeystKgVms0GhH48eMbz/Pc+MREA3eqqenEtooKkp0EAKlUChazuSYai50bGh4W03wvRkZCFrO5Sl1LsoxAIHCopa1tVqPx/IULjkQiEeQ4DrIsw7+8vFtN0o3x0dGVB/fv7y9lFwBg8u3br36/v8ZsNuf7Swg4jgMhxHfm7NmD6tCowesBUkolxhhMJlPebKgTKKdSko7b+p0uNR4AYDAYyh8dQggBKY7J9KQXAUQpieRfhpsxlvub5YEVEFgQEKR0ySi7KYQQQrMeRb1frRAhf6Ora8fCwoKQkuU/KUrhdrtjiqLwpRQqlPJOp3PXgdpaplAKq8XCORyODQIAC7OzRz9NT7+ijAlC1kNbyKfs5smyjF/R6FpcFE+2tbfPa2R8eP++we/3v05KUrWjqgrFypZlGbF4PBiJRI51dHZ+Ldr9+ZmZQ4uLiy9/J5M1dkHQAEuShFg8vra+sdHY0trqLXecAABflpaOeTyeUUmWq7dXVEAUxW/RWKz5SkfHDP4nng8N1T9+9GhnObl/AQgqdZbz5HyFAAAAAElFTkSuQmCC);
background-repeat: no-repeat;
width: 26px;
height: 36px;
}

.divup {
-ms-transform: rotate(180deg);
-webkit-transform: rotate(180deg);
transform: rotate(180deg);
margin-right: 5px;
margin-top: -5px;
}

.divright {
-ms-transform: rotate(270deg);
-webkit-transform: rotate(270deg);
transform: rotate(270deg);
margin-left: 5px;
margin-top: -5px;
}

.divdown {
margin-left: 5px;
margin-bottom: -5px;
}

.divleft {
-ms-transform: rotate(90deg);
-webkit-transform: rotate(90deg);
transform: rotate(90deg);
margin-left: -5px;
margin-bottom: -5px;
margin-right: 10px;
}

#btnstop {
width: 41px;
height: 41px;
padding-bottom: 6px;
}

.btnstart {
font-size: small;
background-color: green;
border-top-width: 2px;
border-left-width: 2px;
border-top-style: groove;
border-right-style: groove;
border-bottom-style: groove;
border-left-style: groove;
border-top-color: aliceblue;
border-left-color: aliceblue;
padding: 2px 12px 5px 12px;
line-height: 0px;
cursor: default;
}
</style>
)rawliteral";