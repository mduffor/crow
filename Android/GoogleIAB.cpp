

// https://developer.android.com/google/play/billing/api.html#consume

/*
Here is the basic flow for purchasing a consumable managed product:

Call the getBuyIntent method to launch a purchase flow.

Inspect the returned Bundle from Google Play to determine whether the purchase completed successfully.

If the purchase was successful, consume the purchase by calling the consumePurchase method.

Inspect the response code from Google Play to determine whether the consumption completed successfully.

If the consumption was successful, provision the product in your application.




Subsequently, when the user starts up or logs in to your application, you should check
if the user owns any outstanding consumable in-app products; if so, make sure to consume
and provision those items. Here's the recommended application startup flow if you implement
consumable in-app products in your application:

Send a getPurchases request to query the owned in-app products for the user.

If there are any consumable in-app products, consume the items by calling consumePurchase.
This step is necessary because the application might have completed the purchase order for
the consumable product, but stopped or got disconnected before the application had the
chance to send a consumption request.

Inspect the response code from Google Play to determine whether the consumption completed successfully.

If the consumption was successful, provision the product in your application.
*/
/*
Note:  Need to write a keystore class, to hold app public key, ad keys, etc.



QueryProducts

PurchaseProduct
  getBuyIntent
    returns Bundle that contains a PendingIntent
    startIntentSenderForResult ()

OnPurchaseSuccessful (both internal callback and event)
  Triggered via Intent sent to your onActivityResult.  Validate locally.  Record event.
  This is where you update game state for non-consumables
  For consumables, ConsumeProduct () will automatically be called.

OnPurchaseFailed
  Record event to track canceled purchases.

Initialize (call after game state exists and OnPurchase* OnConsume* callbacks are set up)
  isBillingSupported
  getSkuDetails (fill out cache that QueryProducts returns)
  getPurchases.  Call OnPurchaseSuccessful (or OnPurchaseFailed?) for each
    This needs to fill out internal


ConsumeProduct
  consumePurchase

OnConsumeSuccessful
  This is where you would actually change game state.
  Record event.

OnConsumeFailed
  Record event.

*/
