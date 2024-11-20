%% This file converts to convert audio file (.mp3 or .wav) to array of samples in .c file

function Audio2c(inputFile,outputAudio)

    [audio,Fs] = audioread(inputFile); %audio samples and sampling rate
    channels = size(audio,2);

    %For stereo audio
    if channels == 2
        arraySize = size(audio,1); 
        norm_audio = audio/max(abs(audio(:))); %normalise samples to range between -1 and 1
        %Convert samples to 32-bit integers
        if max(abs(audio(:))) == 2147483648
            new_audio = int32(2147483648*norm_audio);
        else 
            new_audio = int32(2147483647*norm_audio);
        end 
    
        %Create .c file and write samples to an array
        fd = fopen([outputAudio '.c'], 'wt');
        fprintf(fd,'//2-channel audio in the form of a C array. The audio samples are represented as 32-bit signed integers.\n');
        fprintf(fd, '//Sample rate: %d Hz\n\n', Fs);
        fprintf(fd,['const signed int ' outputAudio '[%d][2] = {\n'], arraySize);
        for i = 1:(arraySize-1)
            fprintf(fd, '{%d,%d},', new_audio(i,1), new_audio(i,2));
             
            % Start new line after every 10 samples
            if mod(i, 10) == 0
                fprintf(fd, '\n');
            end 
        end
        fprintf(fd, '{%d,%d}\n};', new_audio(end,1),new_audio(end,2)); %Write last sample and close array
    
        fclose(fd);
    end

    %For mono audio
    if channels == 1
        arraySize = numel(audio);
        norm_audio = audio/max(abs(audio)); %normalise samples to range between -1 and 1
        if max(abs(audio)) == 2147483648
            new_audio = int32(2147483648*norm_audio);
        else 
            new_audio = int32(2147483647*norm_audio);
        end 
    
        fd = fopen([outputAudio '.c'], 'wt');
        fprintf(fd,'//Audio data in the form of a C array. The audio samples are represented as 32-bit signed integers.\n');
        fprintf(fd, '//Sample rate: %d Hz\n\n', Fs);
        fprintf(fd,['const signed int ' outputAudio ' [%d] = \n{'], arraySize);
        for i = 1:(arraySize-1)
            fprintf(fd, '%d,', new_audio(i));
            
            % Start new line after every 10 samples
            if mod(i, 10) == 0
                fprintf(fd, '\n');
            end
        end
        fprintf(fd, '%d};', new_audio(end)); %Write last sample and close array
    
        fclose(fd);
    end
end
