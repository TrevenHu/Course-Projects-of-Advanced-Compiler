; ModuleID = 'test4.c'
source_filename = "test4.c"
target datalayout = "e-m:e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

%struct.TestStruct = type { i8, i32, float, i8*, i32, double }

; Function Attrs: noinline nounwind optnone uwtable
define i32 @foo(i32) #0 {
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca [2 x float], align 4
  %6 = alloca float*, align 8
  %7 = alloca i64, align 8
  %8 = alloca i8*, align 8
  %9 = alloca [6 x %struct.TestStruct], align 16
  %10 = alloca i32, align 4
  %11 = alloca i32, align 4
  %12 = alloca i32, align 4
  %13 = alloca i32, align 4
  %14 = alloca i32, align 4
  store volatile i32 %0, i32* %2, align 4
  store volatile i32 10, i32* %3, align 4
  %15 = load volatile i32, i32* %3, align 4
  %16 = shl i32 %15, 2
  store i32 %16, i32* %4, align 4
  %17 = getelementptr inbounds [2 x float], [2 x float]* %5, i32 0, i32 0
  store float* %17, float** %6, align 8
  %18 = getelementptr inbounds [2 x float], [2 x float]* %5, i64 0, i64 0
  store float 1.000000e+01, float* %18, align 4
  %19 = getelementptr inbounds [2 x float], [2 x float]* %5, i64 0, i64 0
  %20 = load float, float* %19, align 4
  %21 = getelementptr inbounds [2 x float], [2 x float]* %5, i64 0, i64 1
  store float %20, float* %21, align 4
  store i64 4294967280, i64* %7, align 8
  %22 = bitcast i64* %7 to i8*
  store i8* %22, i8** %8, align 8
  store i32 0, i32* %10, align 4
  br label %23

; <label>:23:                                     ; preds = %50, %1
  %24 = load i32, i32* %10, align 4
  %25 = icmp slt i32 %24, 6
  br i1 %25, label %26, label %53

; <label>:26:                                     ; preds = %23
  %27 = load i32, i32* %10, align 4
  %28 = srem i32 %27, 2
  %29 = sext i32 %28 to i64
  %30 = getelementptr inbounds [2 x float], [2 x float]* %5, i64 0, i64 %29
  %31 = bitcast float* %30 to i8*
  %32 = load i32, i32* %10, align 4
  %33 = sext i32 %32 to i64
  %34 = getelementptr inbounds [6 x %struct.TestStruct], [6 x %struct.TestStruct]* %9, i64 0, i64 %33
  %35 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %34, i32 0, i32 3
  store volatile i8* %31, i8** %35, align 8
  %36 = load volatile i32, i32* %2, align 4
  %37 = load i32, i32* %10, align 4
  %38 = add i32 %36, %37
  %39 = load i32, i32* %10, align 4
  %40 = sext i32 %39 to i64
  %41 = getelementptr inbounds [6 x %struct.TestStruct], [6 x %struct.TestStruct]* %9, i64 0, i64 %40
  %42 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %41, i32 0, i32 1
  store volatile i32 %38, i32* %42, align 4
  %43 = load i32, i32* %10, align 4
  %44 = sext i32 %43 to i64
  %45 = getelementptr inbounds [6 x %struct.TestStruct], [6 x %struct.TestStruct]* %9, i64 0, i64 %44
  %46 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %45, i32 0, i32 1
  %47 = load volatile i32, i32* %46, align 4
  %48 = load i32, i32* %11, align 4
  %49 = add nsw i32 %48, %47
  store i32 %49, i32* %11, align 4
  br label %50

; <label>:50:                                     ; preds = %26
  %51 = load i32, i32* %10, align 4
  %52 = add nsw i32 %51, 1
  store i32 %52, i32* %10, align 4
  br label %23

; <label>:53:                                     ; preds = %23
  %54 = load i32, i32* %4, align 4
  %55 = load i32, i32* %11, align 4
  %56 = add i32 %54, %55
  %57 = load i32, i32* %12, align 4
  %58 = srem i32 %57, 16
  %59 = add i32 %56, %58
  store i32 %59, i32* %4, align 4
  %60 = load volatile i32, i32* %2, align 4
  %61 = icmp ugt i32 %60, 5
  br i1 %61, label %62, label %77

; <label>:62:                                     ; preds = %53
  %63 = getelementptr inbounds [2 x float], [2 x float]* %5, i64 0, i64 1
  %64 = load float, float* %63, align 4
  %65 = fcmp ogt float %64, 5.000000e+00
  br i1 %65, label %66, label %77

; <label>:66:                                     ; preds = %62
  %67 = load volatile i32, i32* %3, align 4
  %68 = add i32 %67, 5
  store volatile i32 %68, i32* %3, align 4
  %69 = load i32, i32* %4, align 4
  %70 = add i32 %69, 10
  store i32 %70, i32* %4, align 4
  %71 = load i8*, i8** %8, align 8
  %72 = load volatile i32, i32* %2, align 4
  %73 = urem i32 %72, 4
  %74 = zext i32 %73 to i64
  %75 = getelementptr inbounds [6 x %struct.TestStruct], [6 x %struct.TestStruct]* %9, i64 0, i64 %74
  %76 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %75, i32 0, i32 3
  store volatile i8* %71, i8** %76, align 8
  br label %82

; <label>:77:                                     ; preds = %62, %53
  %78 = load volatile i32, i32* %3, align 4
  %79 = add i32 %78, 50
  store volatile i32 %79, i32* %3, align 4
  %80 = load i32, i32* %4, align 4
  %81 = add i32 %80, 100
  store i32 %81, i32* %4, align 4
  br label %82

; <label>:82:                                     ; preds = %77, %66
  %83 = load volatile i32, i32* %3, align 4
  switch i32 %83, label %90 [
    i32 10, label %84
    i32 15, label %87
  ]

; <label>:84:                                     ; preds = %82
  %85 = load volatile i32, i32* %3, align 4
  %86 = mul i32 %85, 2
  store volatile i32 %86, i32* %3, align 4
  br label %98

; <label>:87:                                     ; preds = %82
  %88 = load i32, i32* %4, align 4
  %89 = mul i32 %88, 2
  store i32 %89, i32* %4, align 4
  br label %98

; <label>:90:                                     ; preds = %82
  %91 = load volatile i32, i32* %2, align 4
  %92 = urem i32 %91, 4
  %93 = zext i32 %92 to i64
  %94 = getelementptr inbounds [6 x %struct.TestStruct], [6 x %struct.TestStruct]* %9, i64 0, i64 %93
  %95 = getelementptr inbounds %struct.TestStruct, %struct.TestStruct* %94, i32 0, i32 3
  %96 = load volatile i8*, i8** %95, align 8
  %97 = ptrtoint i8* %96 to i32
  store i32 %97, i32* %4, align 4
  br label %98

; <label>:98:                                     ; preds = %90, %87, %84
  %99 = load volatile i32, i32* %3, align 4
  %100 = icmp ugt i32 %99, 50
  br i1 %100, label %101, label %103

; <label>:101:                                    ; preds = %98
  %102 = load volatile i32, i32* %3, align 4
  br label %107

; <label>:103:                                    ; preds = %98
  %104 = load i32, i32* %4, align 4
  %105 = load volatile i32, i32* %2, align 4
  %106 = add i32 %104, %105
  br label %107

; <label>:107:                                    ; preds = %103, %101
  %108 = phi i32 [ %102, %101 ], [ %106, %103 ]
  store i32 %108, i32* %4, align 4
  %109 = load i32, i32* %4, align 4
  %110 = load volatile i32, i32* %2, align 4
  %111 = add i32 %109, %110
  %112 = urem i32 %111, 6
  store i32 %112, i32* %13, align 4
  store i32 0, i32* %14, align 4
  %113 = load i32, i32* %13, align 4
  switch i32 %113, label %119 [
    i32 1, label %114
    i32 2, label %115
    i32 3, label %116
    i32 4, label %117
    i32 5, label %118
  ]

; <label>:114:                                    ; preds = %107
  store i32 1, i32* %14, align 4
  br label %119

; <label>:115:                                    ; preds = %107
  store i32 2, i32* %14, align 4
  br label %119

; <label>:116:                                    ; preds = %107
  store i32 3, i32* %14, align 4
  br label %119

; <label>:117:                                    ; preds = %107
  store i32 4, i32* %14, align 4
  br label %119

; <label>:118:                                    ; preds = %107
  store i32 5, i32* %14, align 4
  br label %119

; <label>:119:                                    ; preds = %107, %118, %117, %116, %115, %114
  %120 = load i32, i32* %14, align 4
  ret i32 %120
}

attributes #0 = { noinline nounwind optnone uwtable "correctly-rounded-divide-sqrt-fp-math"="false" "disable-tail-calls"="false" "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-jump-tables"="false" "no-nans-fp-math"="false" "no-signed-zeros-fp-math"="false" "no-trapping-math"="false" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+fxsr,+mmx,+sse,+sse2,+x87" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.module.flags = !{!0}
!llvm.ident = !{!1}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{!"clang version 5.0.1 (tags/RELEASE_501/final 322011)"}
