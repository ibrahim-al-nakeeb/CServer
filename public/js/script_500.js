const root = document.documentElement;
const eyef = document.getElementById('eyef');
let cx = document.getElementById("eyef").getAttribute("cx");
let cy = document.getElementById("eyef").getAttribute("cy");

document.addEventListener("mousemove", evt => {
	const x = evt.clientX / innerWidth;
	const y = evt.clientY / innerHeight;

	root.style.setProperty("--mouse-x", x);
	root.style.setProperty("--mouse-y", y);
  
	cx = 115 + 30 * x;
	cy = 50 + 30 * y;
	eyef.setAttribute("cx", cx);
	eyef.setAttribute("cy", cy);
});

document.addEventListener("touchmove", touchHandler => {
	const x = touchHandler.touches[0].clientX / innerWidth;
	const y = touchHandler.touches[0].clientY / innerHeight;

	root.style.setProperty("--mouse-x", x);
	root.style.setProperty("--mouse-y", y);
});
