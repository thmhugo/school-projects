var btnPopup = document.getElementById('btnPopup');
var overlay = document.getElementById('overlay');
var btnClose = document.getElementById('btnClose');

btnPopup.addEventListener('click',openModal);

btnClose.addEventListener('click',closePopup);

function openModal(){
  overlay.style.display = 'block';
}

function closePopup() {
  overlay.style.display='none';
}
