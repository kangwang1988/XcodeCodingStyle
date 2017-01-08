//
//  ViewController.m
//  XcodeCodingStyleDemo
//
//  Created by KyleWong on 26/10/2016.
//  Copyright © 2016 KyleWong. All rights reserved.
//

#import "ViewController.h"
#import "UserModel.h"

@interface XXViewController ()
@end

@implementation XXViewController
- (instancetype)init{
    if(self = [super init]){
        //[KWLT]不得在init/dealloc中向其发送消息
        [self func1];
    }
    return self;
}
- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setBackgroundColor:[UIColor redColor]];
    user_Model *model = [user_Model alloc];
    //[KWLT]多参数(超过3)时，分行展示且冒号对齐
    model = [model initWithMAC:nil
                                            AzIp:nil
                         AzDns:nil
                         Token:nil
                         Email:nil];
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)dealloc{
    [self func2];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    //[KWLT]语法糖在clang中已经预先被处理了
    self.view;
    self.version;
    int x = 100;
    //[KWLT]If表达式条件测试
    if(!10)
    {
        NSLog(@"1");
    }
    if(10)
    {
        NSLog(@"2");
    }
    if(true)
    {
        NSLog(@"3");
    }
    if(false)
    {
        NSLog(@"4");
    }
    if(x=10){
        NSLog(@"5");
    }
}

- (void)func1{
    
}

- (void)func2{
    
}
@end
