"use-strict";

function jsFib(a) {
  if (a == 0 || a == 1) {
    return a;
  }
  return jsFib(a - 2) + jsFib(a - 1);
}

document.querySelector(".mybutton").addEventListener("click", function () {
  console.log("CPP FIB");
  console.time("CPP FIB");
  Module._fib(15) + Module._fib(10);
  console.timeEnd("CPP FIB");

  console.log("JS FIB");
  console.time("JS FIB");
  jsFib(15) + jsFib(10);
  console.timeEnd("JS FIB");
});
