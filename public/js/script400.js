$('body').mousemove(function(event) {
	const e = $('.eye');
	const x = (e.offset().left) + (e.width() / 2);
	const y = (e.offset().top) + (e.height() / 2);
	const rad = Math.atan2(event.pageX - x, event.pageY - y);
	const rot = (rad * (180 / Math.PI) * -1) + 180;
	e.css({
		'-webkit-transform': 'rotate(' + rot + 'deg)',
		'transform': 'rotate(' + rot + 'deg)'
	});
});