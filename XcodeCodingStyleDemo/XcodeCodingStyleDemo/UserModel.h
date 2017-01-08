//
//  UserModel.h
//  XcodeCodingStyleDemo
//
//  Created by KyleWong on 26/10/2016.
//  Copyright © 2016 KyleWong. All rights reserved.
//

#import <Foundation/Foundation.h>
//[KWLT]类名以XX开头，不包含下划线。
@interface user_Model : NSObject{
    NSString *_ivar1;
    //[KWLT]ivar以下划线开头
    NSString *ivar2;
}
//[KWLT]string使用copy属性
@property (nonatomic,strong) NSString *name;
@property (nonatomic,strong) NSArray *friends;
//[KWLT]使用内置数据类型
@property (assign,nonatomic) int age;
@property (assign,nonatomic) bool female;
- (instancetype)initWithName:(NSString *)name age:(NSInteger)age female:(bool)isFemale;
//[KWLT]多参数(如超过3)时，分行展示且冒号对齐
- (instancetype)initWithMAC:(NSString *)mac
                       AzIp:(NSString *)az_ip
                      AzDns:(NSString *)az_dns
                      Token:(NSString *)token
                      Email:(NSString *)email;
//[KWLT]变量名长度不得超过15个字符
- (void)varNameTest:(NSInteger)numberOfPeopleOnTheUsOlympicTeam;
//[KWLT]方法长度不超过500行
- (void)methodLenTest:(long long)nSize;
//[KWLT]函数参数使用内置数据类型
- (void)writeVideoFrameWithData:(NSData *)frameData timeStamp:(int)timeStamp;
@end
