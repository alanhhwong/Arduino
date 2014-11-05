/* Assign actions */
$('.product-quantity input').change( function() {
  updateQuantity(this);
});

$('.product-removal button').click( function() {
  removeItem(this);
});

$("#checkout_form").submit(function () {
  //products json
  var products = [];
  $('.product').each(function () {
    var title = $(this).find('.product-title').text();
    var quantity = $(this).find('.product-quantity')[0].firstElementChild.value;
    var price = $(this).find('.product-line-price').text();

    var item = {};
    item["t"] = title;
    item["q"] = quantity;
    item["p"] = price;

    console.log(quantity);
    products.push(item);
  });

  var plist = $('<input>').attr({
    type: 'hidden',
    name: 'products',
    value: JSON.stringify(products)
  });

  //total cost
  var total = $('<input>').attr({
    type: 'hidden',
    name: 'total',
    value: $('#cart-total').text()
  });

  $("#checkout_form").append($(plist));
  $("#checkout_form").append($(total));

  alert("Order has been submitted!");
});

/* Recalculate cart */
function recalculateCart()
{
  var total = 0;
  
  /* Sum up row totals */
  $('.product').each(function () {
    total += parseFloat($(this).children('.product-line-price').text());
  });
  
  /* Update totals display */
  $('.totals-value').fadeOut(fadeTime, function() {
    $('#cart-total').html(total.toFixed(2));
    if(total == 0){
      $('.checkout').fadeOut(fadeTime);
    }else{
      $('.checkout').fadeIn(fadeTime);
    }
    $('.totals-value').fadeIn(fadeTime);
  });
}


/* Update quantity */
function updateQuantity(quantityInput)
{
  /* Calculate line price */
  var productRow = $(quantityInput).parent().parent();
  var price = parseFloat(productRow.children('.product-price').text());
  var quantity = $(quantityInput).val();
  var linePrice = price * quantity;
  
  /* Update line price display and recalc cart totals */
  productRow.children('.product-line-price').each(function () {
    $(this).fadeOut(fadeTime, function() {
      $(this).text(linePrice.toFixed(2));
      recalculateCart();
      $(this).fadeIn(fadeTime);
    });
  });  
}


/* Remove item from cart */
function removeItem(removeButton)
{
  /* Remove row from DOM and recalc cart total */
  var productRow = $(removeButton).parent().parent();
  productRow.slideUp(fadeTime, function() {
    productRow.remove();
    recalculateCart();
  });
}