//
//  ViewController.h
//  XcodeCodingStyleDemo
//
//  Created by KyleWong on 26/10/2016.
//  Copyright © 2016 KyleWong. All rights reserved.
//

#import <UIKit/UIKit.h>

//[KWLT]枚举使用NS_ENUM
typedef enum {
    CStyleEnumTypeDefault,
}CStyleEnumType;

typedef NS_ENUM(NSInteger){
    OCStyleEnumTypeDefault,
}OCStyleEnumType;

@class XXViewController;
@protocol XXViewControllerDelegate <NSObject>
- (void)didLoadOfViewController:(XXViewController *)vc;
@end

@interface XXViewController : UIViewController
//[KWLT]delegate为weak
@property(nonatomic,strong) id<XXViewControllerDelegate> delegate;
//[KWLT]delegate书写格式
@property(nonatomic,weak) id< UITextFieldDelegate>  tfDelegate;
//[KWLT]属性使用nonatomic
@property (copy) NSString *hint;
- (NSInteger)version;
@end
